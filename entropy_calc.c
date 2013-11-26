#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <argp.h>
#include <math.h>
#include "log.h"

/** Calculator version */
const char *argp_program_version = "entropy_calc 0.2";
/** Email to send calculator bug reports */
const char *argp_program_bug_address = "<dmitry.zbarski@gmail.com>";

/** Program documentation. */
static char doc[] = "Entropy Adaptive Coding - entropy calculator";

/** A description of the arguments we accept. */
static char args_doc[] = " -i INPUT_FILE";

/** The options calculator accpets */
static struct argp_option options[] = {
    {"debug",       'd', 0,            OPTION_ARG_OPTIONAL,  "Produce debug output" },
    {"input",       'i', "FILE",       0,                    "Input file" },
    {"block-size",  'b', "BLOCK_SIZE", 0,                    "Block size" },
    { 0 }
};
     
/** \brief Calculator arguments structure */
struct calc_arguments {
    int debug;                  /**< debug parameter */
    int block_size;             /**< block size */
    char *input_file;           /**< input file parameter */
};
     
/** Parse a single option of calculator */
static error_t calc_parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct calc_arguments *arguments = state->input;
     
    switch (key)
    {
    case 'd':
        arguments->debug = 1;
        break;
    case 'i':
        arguments->input_file = arg;
        break;
    case 'b':
        arguments->block_size = atoi(arg);
        break;
     
    case ARGP_KEY_ARG:
        if(state->arg_num >= 1)
            argp_usage (state);
        break;
     
    case ARGP_KEY_END:
        if(arguments->input_file == 0)
            argp_usage (state);
        break;
     
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/** Calculator argp parser */
static struct argp argp = { options, calc_parse_opt, args_doc, doc };

int count_set_bits(uint8_t byte)
{
    int result = 0;
    for(int i=0;i<8;i++) {
        result += byte >> i & 0x1;
    }
    return result;
}

int main(int argc, char *argv[])
{
    struct calc_arguments arguments;
     
    /* Default values. */
    arguments.debug = 0;
    arguments.block_size = 0;
    arguments.input_file = 0;
    
    /* Parse our arguments; every option seen by calc_parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    log_debug = arguments.debug;
    
    PRINT_DEBUG("INPUT_FILE = %s\nBLOCK_SIZE = %d bytes\nDEBUG = %s\n",
                arguments.input_file,
                arguments.block_size,
                arguments.debug ? "yes" : "no");
    
    FILE *infile = fopen(arguments.input_file,"r");
    if(infile == NULL) {
        error(1, errno, "Could not open input file");
    }

    if(arguments.block_size == 0) {
        fseek(infile,0L,SEEK_END);
        arguments.block_size = ftell(infile);
        fseek(infile,0L,SEEK_SET);
    }

    uint8_t *buffer = malloc(sizeof(uint8_t) * arguments.block_size);
    if(buffer == NULL) {
        error(1,errno, "Could not allocate memory for buffer");
    }

    while(!feof(infile)) {
        int buffer_size = fread(buffer,sizeof(uint8_t),arguments.block_size,infile);
        if(buffer_size == 0)
            continue;
        long counters[2] = {0L,0L};
        double probs[2] = {0.0,0.0};
        double summands[2] = {0.0,0.0};
        for(int i = 0; i < buffer_size; i++) {
            counters[0] += 8 - count_set_bits(buffer[i]);
            counters[1] += count_set_bits(buffer[i]);
        }
        probs[0] = (double)counters[0] / (counters[0] + counters[1]);
        probs[1] = (double)counters[1] / (counters[0] + counters[1]);
        summands[0] = probs[0] == 0 ? 0 : -1.0 * probs[0] * log2(probs[0]);
        summands[1] = probs[1] == 0 ? 0 : -1.0 * probs[1] * log2(probs[1]);
        PRINT_DEBUG("0 => %ld, 1 => %ld, p0 => %f, p1 => %f, s0 => %f, s1 => %f\n",counters[0],counters[1],probs[0],probs[1],summands[0],summands[1]);
        printf("%f\n", summands[0] + summands[1]);
    }
    
    return EXIT_SUCCESS;
}
