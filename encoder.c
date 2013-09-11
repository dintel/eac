#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <argp.h>
#include "bit_string.h"
#include "bit_string_writer.h"
#include "lz77.h"
#include "log.h"

#define BLOCK_SIZE 8192
#define INITIAL_NW 32768
#define DEFAULT_WINDOW_SIZE 32768

const char *argp_program_version = "eac_encode 0.1";
const char *argp_program_bug_address = "<dmitry.zbarski@gmail.com>";

/* Program documentation. */
static char doc[] = "Entropy Adaptive Coding - encoder";

/* A description of the arguments we accept. */
static char args_doc[] = " -i INPUT_FILE -o OUTPUT_FILE";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose",     'v', 0,      OPTION_ARG_OPTIONAL,  "Produce verbose output" },
    {"debug",       'd', 0,      OPTION_ARG_OPTIONAL,  "Don't produce any output" },
    {"eac",         'e', 0,      OPTION_ARG_OPTIONAL,  "Use Adaptive Entropy Coding" },
    {"window-size", 'n', "FILE", 0,                    "LZ77 window size (ignored when --eac is used)" },
    {"input",       'i', "FILE", 0,                    "Input file" },
    {"output",      'o', "FILE", 0,                    "Output file" },
    { 0 }
};
     
/* Used by main to communicate with parse_opt. */
struct arguments {
    int debug, verbose, eac;
    size_t window_size;
    char *input_file;
    char *output_file;
};
     
/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;
     
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

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

bit_string_t *convert(uint8_t *src, size_t size)
{
    bit_string_t *result = bit_string_init(size * 8);
    for(int i = 0; i < size; ++i) {
        for(int j = 7; j >= 0; --j) {
            bit_string_append_bit(result, src[i] >> j & 1);
        }
    }
    return result;
}

int main(int argc, char *argv[])
{
    struct arguments arguments;
     
    /* Default values. */
    arguments.debug = 0;
    arguments.verbose = 0;
    arguments.eac = 0;
    arguments.input_file = 0;
    arguments.output_file = 0;
    arguments.window_size = DEFAULT_WINDOW_SIZE;
     
    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    log_verbose = arguments.verbose;
    log_debug = arguments.debug;
    
    PRINT_DEBUG("INPUT_FILE = %s\nOUTPUT_FILE = %s\nWINDOW_SIZE = %zu\nEAC = %s\nVERBOSE = %s\nDEBUG = %s\n",
                arguments.input_file,
                arguments.output_file,
                arguments.window_size,
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
    
    uint8_t *buffer = malloc(sizeof(uint8_t) * BLOCK_SIZE);
    int i = 1;
    int best_nw;
    bit_string_t *buffer_string, *tmp_string, *best_string;
    int buffer_size;
    long file_size = 0, compressed_size = 0;
    bit_string_writer_t *writer = bit_string_writer_init(outfile);
    while(!feof(file)) {
        buffer_size = fread(buffer,sizeof(uint8_t),BLOCK_SIZE,file);
        if(buffer_size == 0) {
            continue;
        }
        PRINT_VERBOSE("Encoding block %d size %d bits\n",i++,buffer_size * 8);
        buffer_string = convert(buffer,buffer_size);
        if(arguments.eac) {
            best_string = 0;
            best_nw = 0;
            for(int i = 2; i <= INITIAL_NW && i < buffer_string->offset; i *= 2 ) {
                tmp_string = lz77_encode(buffer_string,i);
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
            compressed_size += best_string->offset;
            file_size += buffer_string->offset;
            bit_string_writer_write(writer,best_string);
            bit_string_destroy(best_string);
        } else {
            best_string = lz77_encode(buffer_string,arguments.window_size);
            PRINT_VERBOSE("Compressed using window size %zu compressed size %zu ratio %f\n", arguments.window_size, best_string->offset, (float)buffer_string->offset / best_string->offset);
            compressed_size += best_string->offset;
            file_size += buffer_string->offset;
            bit_string_writer_write(writer,best_string);
            bit_string_destroy(best_string);
        }
    }

    compressed_size += bit_string_writer_flush(writer);
    printf("File size %ld compressed file size %ld (ratio %f)\n",file_size,compressed_size, (double)file_size / compressed_size);
    
    return EXIT_SUCCESS;
}
