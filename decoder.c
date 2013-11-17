#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <argp.h>
#include "bit_string.h"
#include "bit_string_writer.h"
#include "lz77.h"
#include "log.h"

#define DEFAULT_BLOCK_SIZE 8192
#define MAX_NW 32768
#define MAX_NW_DELTA 15
#define NW_DELTA_BITS 5
#define MAX_RATIO 400

/** Decoder version  */
const char *argp_program_version = "eac_decode 0.1";
/** Email to send decoder bug reports */
const char *argp_program_bug_address = "<dmitry.zbarski@gmail.com>";

/** Program documentation. */
static char doc[] = "Entropy Adaptive Coding - decoder";

/** A description of the arguments we accept. */
static char args_doc[] = " -i INPUT_FILE -o OUTPUT_FILE";

/** The options decoder accepts */
static struct argp_option options[] = {
    {"verbose",    'v', 0,            OPTION_ARG_OPTIONAL,  "Produce verbose output" },
    {"debug",      'd', 0,            OPTION_ARG_OPTIONAL,  "Don't produce any output" },
    {"eac",        'e', 0,            OPTION_ARG_OPTIONAL,  "Use Adaptive Entropy Coding" },
    {"input",      'i', "FILE",       0,                    "Input file" },
    {"output",     'o', "FILE",       0,                    "Output file" },
    {"block-size", 'b', "BLOCK_SIZE", 0,                    "Block size"},
    { 0 }
};

/** \brief Decoder arguments structure */
struct dec_arguments {
    int debug;                  /**< debug parameter */
    int verbose;                /**< verbose parmeter */
    int eac;                    /**< eac parameter */
    int block_size;             /**< block size */
    char *input_file;           /**< input file parameter */
    char *output_file;          /**< output file parameter */
};
     
/** Parse a single option of decoder */
static error_t dec_parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct dec_arguments *arguments = state->input;
     
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

/** Decoder argp parser */
static struct argp argp = { options, dec_parse_opt, args_doc, doc };

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
    bit_string_t *result = bit_string_init(size * 8);
    for(int i = 0; i < size; ++i) {
        for(int j = 7; j >= 0; --j) {
            bit_string_append_bit(result, src[i] >> j & 1);
            //PRINT_DEBUG("read bit %x (%x)\n",src[i] >> j & 1,result->data[result->offset / 8 - 1]);
        }
    }
    return result;
}

/**
 * \brief Calculate new window size
 *
 * Calculate new window size based on current window size and delta
 * value. Delta value is how many bits should be shifted right/left.
 * \param prev current window size
 * \param delta delta of window size \f$\delta = \log_2\left(\frac{\max\{nw_{n-1},nw_n\}}{\min\{nw_{n-1},nw_n\}}\right)\f$
 * \return new window size
 */
int delta_nw_inv(int prev, uint8_t delta)
{
    if(delta > MAX_NW_DELTA) {
        return prev >> (delta - MAX_NW_DELTA);
    }
    return prev << delta;
}

int main(int argc, char *argv[])
{
    struct dec_arguments arguments;
     
    /* Default values. */
    arguments.debug = 0;
    arguments.verbose = 0;
    arguments.eac = 0;
    arguments.input_file = 0;
    arguments.output_file = 0;
    arguments.block_size = DEFAULT_BLOCK_SIZE;
    
    /* Parse our arguments; every option seen by dec_parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    log_verbose = arguments.verbose;
    log_debug = arguments.debug;

    PRINT_DEBUG("INPUT_FILE = %s\nOUTPUT_FILE = %s\nBLOCK_SIZE = %d bytes\nEAC = %s\nVERBOSE = %s\nDEBUG = %s\n",
                arguments.input_file,
                arguments.output_file,
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
    
    uint8_t *buffer;
    bit_string_t *bs, *tmp, *window = NULL;
    bit_string_writer_t *writer = bit_string_writer_init(outfile);
    long file_size = 0, decompressed_size = 0;
    size_t enc_size, buffer_size, window_size;
    int i = 1;
    uint8_t byte;

    if(arguments.eac) {
        bs = bit_string_init(arguments.block_size * 8);
        buffer = malloc(sizeof(uint8_t) * arguments.block_size * 8);

        while(!feof(file) || bs->offset >= 8) {
            buffer_size = fread(buffer,sizeof(uint8_t),arguments.block_size * 8 - bs->offset / 8 - 1,file);
            if(buffer_size) {
                file_size += buffer_size * 8;
                tmp = convert(buffer,buffer_size);
                bit_string_concat_and_destroy(bs,tmp);
            }
            tmp = lz77_decode(bs,arguments.block_size * 8,&enc_size,window,&window_size);
            if(tmp == NULL) {
                bit_string_destroy(bs);
                break;
            }
            decompressed_size += tmp->offset;
            PRINT_VERBOSE("Decoded block %d encoded size %zu decoded size %zu\n",i++,enc_size,tmp->offset);
            if(window != NULL)
                bit_string_destroy(window);
            window = bit_string_substr(tmp,0,tmp->offset);
            bit_string_writer_write(writer,tmp);
            bit_string_destroy(tmp);
            if(bs->offset >= (enc_size + NW_DELTA_BITS) && arguments.eac) {
                byte = bit_string_read_byte(bs,enc_size,NW_DELTA_BITS);
                window_size = delta_nw_inv(window_size,byte);
                enc_size += NW_DELTA_BITS;
            }
            if(bs->offset < enc_size)
                tmp = bit_string_substr(bs,enc_size,0);
            else
                tmp = bit_string_substr(bs,enc_size,bs->offset - enc_size);
            bit_string_destroy(bs);
            bs = tmp;
        }
        bit_string_destroy(window);
        bit_string_destroy(bs);
    } else {
        fseek(file,0L,SEEK_END);
        file_size = ftell(file);
        fseek(file,0L,SEEK_SET);
        buffer = malloc(sizeof(uint8_t) * file_size);
        buffer_size = fread(buffer,sizeof(uint8_t),file_size,file);
        bs = convert(buffer,buffer_size);
        tmp = lz77_decode(bs,file_size * MAX_RATIO,&enc_size,NULL,&window_size);
        decompressed_size = tmp->offset;
        file_size *= 8;
        bit_string_writer_write(writer,tmp);
        bit_string_destroy(tmp);
        bit_string_destroy(bs);
    }
    
    decompressed_size += bit_string_writer_flush(writer);
    bit_string_writer_destroy(writer);
    printf("%ld;%ld;%f\n",file_size,decompressed_size, (double)decompressed_size / file_size);
    
    free(buffer);
    fclose(file);
    fclose(outfile);
    return EXIT_SUCCESS;
}
