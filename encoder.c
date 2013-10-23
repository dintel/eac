#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <argp.h>
#include <unistd.h>
#include "bit_string.h"
#include "bit_string_writer.h"
#include "lz77.h"
#include "log.h"

#define DEFAULT_BLOCK_SIZE 8192
#define DEFAULT_WINDOW_SIZE 32768
#define INITIAL_NW 32768
#define MAX_NW_DELTA 15
#define NW_DELTA_BITS 5

/** Encoder version */
const char *argp_program_version = "eac_encode 0.1";
/** Email to send encoder bug reports */
const char *argp_program_bug_address = "<dmitry.zbarski@gmail.com>";

/** Program documentation. */
static char doc[] = "Entropy Adaptive Coding - encoder";

/** A description of the arguments we accept. */
static char args_doc[] = " -i INPUT_FILE -o OUTPUT_FILE";

/** The options encoder accpets */
static struct argp_option options[] = {
    {"verbose",     'v', 0,            OPTION_ARG_OPTIONAL,  "Produce verbose output" },
    {"debug",       'd', 0,            OPTION_ARG_OPTIONAL,  "Don't produce any output" },
    {"eac",         'e', 0,            OPTION_ARG_OPTIONAL,  "Use Adaptive Entropy Coding" },
    {"window-size", 'n', "FILE",       0,                    "LZ77 window size (ignored when --eac is used)" },
    {"input",       'i', "FILE",       0,                    "Input file" },
    {"output",      'o', "FILE",       0,                    "Output file" },
    {"block-size",  'b', "BLOCK_SIZE", 0,                    "Block size"},
    { 0 }
};
     
/** \brief Encoder arguments structure */
struct enc_arguments {
    int debug;                  /**< debug parameter */
    int verbose;                /**< verbose parmeter */
    int eac;                    /**< eac parameter */
    int block_size;             /**< block size */
    size_t window_size;         /**< window size parameter */
    char *input_file;           /**< input file parameter */
    char *output_file;          /**< output file parameter */
};
     
/** Parse a single option of encoder */
static error_t enc_parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct enc_arguments *arguments = state->input;
     
    switch (key)
    {
    case 'd':
        arguments->verbose = 1;
        arguments->debug = 1;
        break;
    case 'v':
        arguments->verbose = 1;
        break;
    case 'i':
        arguments->input_file = arg;
        break;
    case 'o':
        arguments->output_file = arg;
        break;
    case 'n':
        arguments->window_size = atoi(arg);
        break;
    case 'e':
        arguments->eac = 1;
        break;
    case 'b':
        arguments->block_size = atoi(arg);
        break;
     
    case ARGP_KEY_ARG:
        if(state->arg_num >= 1)
            argp_usage (state);
        break;
     
    case ARGP_KEY_END:
        if(arguments->input_file == 0 || arguments->output_file == 0)
            argp_usage (state);
        break;
     
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/** Encoder argp parser */
static struct argp argp = { options, enc_parse_opt, args_doc, doc };

/**
 * \brief Convert byte array into bit string
 *
 * Convert given byte array into newly allocated bit string.
 *
 * <b>NOTE:</b> It is user's responsibility to destroy bit string when it is
 * not needed anymore.
 * \param src source byte array
 * \param size size of byte array
 * \return newly allocated bit string
 */
bit_string_t *convert(uint8_t *src, size_t size)
{
    bit_string_t *result = bit_string_init(size);
    for(int i = 0; i < size; ++i) {
        for(int j = 7; j >= 0; --j) {
            bit_string_append_bit(result, src[i] >> j & 1);
        }
    }
    return result;
}

/**
 * \brief Calculate delta of window sizes
 *
 * Calculate delta between window size changes. Delta value is how many bits
 * should be shifted right/left. \f$\delta = \log_2\left(\frac{\max\{nw_{n-1},nw_n\}}{\min\{nw_{n-1},nw_n\}}\right)\f$
 * \param prev old window size
 * \param next new window size
 * \return delta between window sizes
 */
uint8_t delta_nw(int prev,int next)
{
    if(next >= prev) {
        PRINT_DEBUG("delta_nw window increase %d\n",(int)log2(next / prev));
        return log2(next / prev);
    }
    PRINT_DEBUG("delta_nw window decrease %d\n",(int)log2(prev / next));
    return log2(prev / next) + MAX_NW_DELTA;
}

int main(int argc, char *argv[])
{
    struct enc_arguments arguments;
     
    /* Default values. */
    arguments.debug = 0;
    arguments.verbose = 0;
    arguments.eac = 0;
    arguments.block_size = DEFAULT_BLOCK_SIZE;
    arguments.input_file = 0;
    arguments.output_file = 0;
    arguments.window_size = DEFAULT_WINDOW_SIZE;
     
    /* Parse our arguments; every option seen by enc_parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    log_verbose = arguments.verbose;
    log_debug = arguments.debug;
    
    PRINT_DEBUG("INPUT_FILE = %s\nOUTPUT_FILE = %s\nWINDOW_SIZE = %zu\nBLOCK_SIZE = %d\nEAC = %s\nVERBOSE = %s\nDEBUG = %s\n",
                arguments.input_file,
                arguments.output_file,
                arguments.window_size,
                arguments.block_size,
                arguments.eac ? "yes" : "no",
                arguments.verbose ? "yes" : "no",
                arguments.debug ? "yes" : "no");

    FILE *file = fopen(arguments.input_file,"r");
    if(file == NULL) {
        error(1, errno, "Could not open input file");
    }

    FILE *outfile = fopen(arguments.output_file,"w");
    if(outfile == NULL) {
        error(1, errno, "Could not open output file");
    }
    
    uint8_t *buffer = malloc(sizeof(uint8_t) * arguments.block_size);
    int i = 1;
    int prev_nw = 0,best_nw = -1;
    bit_string_t *buffer_string, *tmp_string, *best_string, *window = NULL;
    int buffer_size;
    long file_size = 0, compressed_size = 0;
    bit_string_writer_t *writer = bit_string_writer_init(outfile);
    while(!feof(file)) {
        buffer_size = fread(buffer,sizeof(uint8_t),arguments.block_size,file);
        if(buffer_size == 0) {
            continue;
        }
        PRINT_VERBOSE("Encoding block %d size %d bits\n",i++,buffer_size * 8);
        buffer_string = convert(buffer,buffer_size);
        if(arguments.eac) {
            prev_nw = best_nw;
            best_string = 0;
            best_nw = 0;
            for(int i = 2; i <= INITIAL_NW; i *= 2 ) {
                tmp_string = lz77_encode(buffer_string,i,window);
                PRINT_DEBUG("Window size %d ratio %f\n",i,(float)tmp_string->offset / buffer_string->offset);
                if(best_string == 0) {
                    best_string = tmp_string;
                    best_nw = i;
                } else if(tmp_string->offset < best_string->offset) {
                    bit_string_destroy(best_string);
                    best_string = tmp_string;
                    best_nw = i;
                } else {
                    bit_string_destroy(tmp_string);
                }
            }
            PRINT_VERBOSE("Best window size %d compressed size %zu ratio %f\n", best_nw, best_string->offset, (float)buffer_string->offset / best_string->offset);
            if(prev_nw != -1)
                bit_string_writer_write_byte(writer,delta_nw(prev_nw,best_nw),NW_DELTA_BITS);
            if(window != NULL)
                bit_string_destroy(window);
            if(buffer_string->offset >= INITIAL_NW)
                window = bit_string_substr(buffer_string,buffer_string->offset - INITIAL_NW,INITIAL_NW);
            else
                window = bit_string_substr(buffer_string,0,buffer_string->offset);
            compressed_size += best_string->offset;
            file_size += buffer_string->offset;
            bit_string_writer_write(writer,best_string);
            bit_string_destroy(best_string);
            bit_string_destroy(buffer_string);
        } else {
            best_string = lz77_encode(buffer_string,arguments.window_size,window);
            PRINT_VERBOSE("Compressed using window size %zu compressed size %zu ratio %f\n", arguments.window_size, best_string->offset, (float)buffer_string->offset / best_string->offset);
            compressed_size += best_string->offset;
            file_size += buffer_string->offset;
            bit_string_writer_write(writer,best_string);
            if(window) {
                bit_string_destroy(window);
            }
            if(buffer_string->offset >= arguments.window_size)
                window = bit_string_substr(buffer_string,buffer_string->offset - arguments.window_size,arguments.window_size);
            else
                window = bit_string_substr(buffer_string,0,buffer_string->offset);
            bit_string_destroy(best_string);
        }
    }
    bit_string_destroy(window);
    
    if(compressed_size == 0) {
        fclose(file);
        fclose(outfile);
        unlink(arguments.output_file);
        printf("File too small to be compressed\n");
        return EXIT_FAILURE;
    }

    compressed_size += bit_string_writer_flush(writer);
    printf("%ld %ld %f\n",file_size,compressed_size, (double)file_size / compressed_size);
    
    fclose(file);
    fclose(outfile);
    return EXIT_SUCCESS;
}
