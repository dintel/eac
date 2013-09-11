#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <argp.h>
#include "bit_string.h"
#include "bit_string_writer.h"
#include "lz77.h"
#include "log.h"

#define BLOCK_SIZE 4096

const char *argp_program_version = "eac_decode 0.1";
const char *argp_program_bug_address = "<dmitry.zbarski@gmail.com>";

/* Program documentation. */
static char doc[] = "Entropy Adaptive Coding - decoder";

/* A description of the arguments we accept. */
static char args_doc[] = " -i INPUT_FILE -o OUTPUT_FILE";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose",  'v', 0,      OPTION_ARG_OPTIONAL,  "Produce verbose output" },
    {"debug",    'd', 0,      OPTION_ARG_OPTIONAL,  "Don't produce any output" },
    {"input",    'i', "FILE", 0,  "Input file" },
    {"output",   'o', "FILE", 0,  "Output file" },
    { 0 }
};
     
/* Used by main to communicate with parse_opt. */
struct arguments {
    int debug, verbose;
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
            PRINT_DEBUG("read bit %x (%x)\n",src[i] >> j & 1,result->data[result->offset / 8 - 1]);
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
    arguments.input_file = 0;
    arguments.output_file = 0;
    
    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    log_verbose = arguments.verbose;
    log_debug = arguments.debug;
     
    PRINT_DEBUG("INPUT_FILE = %s\nOUTPUT_FILE = %s\nVERBOSE = %s\nDEBUG = %s\n",
                arguments.input_file,
                arguments.output_file,
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
    
    uint8_t *buffer = malloc(sizeof(uint8_t) * BLOCK_SIZE * 8);
    bit_string_t *bs = bit_string_init(BLOCK_SIZE * 8), *tmp;
    long file_size = 0, decompressed_size = 0;
    bit_string_writer_t *writer = bit_string_writer_init(outfile);
    size_t enc_size, buffer_size;
    int i = 1;
    while(!feof(file)) {
        buffer_size = fread(buffer,sizeof(uint8_t),BLOCK_SIZE * 8 - bs->offset / 8 - 1,file);
        if(buffer_size) {
            file_size += buffer_size * 8;
            tmp = convert(buffer,buffer_size);
            bit_string_concat_and_destroy(bs,tmp);
        }
        tmp = lz77_decode(bs,BLOCK_SIZE * 8,&enc_size);
        if(tmp == NULL) {
            break;
        }
        decompressed_size += tmp->offset;
        PRINT_VERBOSE("Decoded block %d encoded size %zu decoded size %zu\n",i++,enc_size,tmp->offset);
        bit_string_writer_write(writer,tmp);
        bit_string_destroy(tmp);
        tmp = bit_string_substr(bs,enc_size,bs->offset - enc_size);
        bit_string_destroy(bs);
        bs = tmp;
    }
    
    while(bs->offset >= 8) {
        tmp = lz77_decode(bs,BLOCK_SIZE * 8,&enc_size);
        if(tmp == NULL) {
            bit_string_destroy(bs);
            break;
        }
        decompressed_size += tmp->offset;
        PRINT_VERBOSE("Decoded block %d encoded size %zu decoded size %zu\n",i++,enc_size,tmp->offset);
        bit_string_writer_write(writer,tmp);
        bit_string_destroy(tmp);
        if(bs->offset < enc_size)
            tmp = bit_string_substr(bs,enc_size,0);
        else
            tmp = bit_string_substr(bs,enc_size,bs->offset - enc_size);
        bit_string_destroy(bs);
        bs = tmp;
    }
    
    decompressed_size += bit_string_writer_flush(writer);
    printf("File size %ld decompressed file size %ld (ratio %f)\n",file_size,decompressed_size, (double)file_size / decompressed_size);
    
    return EXIT_SUCCESS;
}
