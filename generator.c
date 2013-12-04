#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <unistd.h>
#include <time.h>
#include "bit_string.h"
#include "bit_string_writer.h"
#include "log.h"

/** Generator version */
const char *argp_program_version = "eac_generator 0.2";
/** Email to send generator bug reports */
const char *argp_program_bug_address = "<dmitry.zbarski@gmail.com>";

/** Program documentation. */
static char doc[] = "Entropy Adaptive Coding - generator";

/** A description of the arguments we accept. */
static char args_doc[] = " -o OUTPUT_FILE -n SIZE";

/** The options generator accpets */
static struct argp_option options[] = {
    {"size",        'n', "FILE",       0, "Size of file in bytes" },
    {"probability", 'p', "NUM",        0, "Probability of 0 (probability of 1 is calculated automatically)" },
    {"output",      'o', "FILE",       0, "Output file name" },
    { 0 }
};

/** \brief Generator arguments structure */
struct generator_arguments {
    size_t size;         /**< file size parameter */
    char *file;          /**< output file parameter */
    double probability;  /**< probability of 0 */
};

/** Parse a single option of generator */
static error_t generator_parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct generator_arguments *arguments = state->input;
     
    switch (key)
    {
    case 'o':
        arguments->file = arg;
        break;
    case 'n':
        arguments->size = atoi(arg);
        break;
    case 'p':
        arguments->probability = atof(arg);
        break;
     
    case ARGP_KEY_ARG:
        if(state->arg_num >= 1)
            argp_usage (state);
        break;
     
    case ARGP_KEY_END:
        if(arguments->file == 0 || arguments->size <= 0 || arguments->probability < 0 || arguments->probability > 1)
            argp_usage (state);
        break;
     
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/** Generator argp parser */
static struct argp argp = { options, generator_parse_opt, args_doc, doc };

int main(int argc, char *argv[])
{
    struct generator_arguments arguments;
     
    /* Default values. */
    arguments.size = 0;
    arguments.file = 0;
    arguments.probability = -1.0;
    
    /* Parse our arguments; every option seen by enc_parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    FILE *file = fopen(arguments.file,"w");
    if(file == NULL) {
        error(1, errno, "Could not open input file");
    }

    bit_string_t *bs = bit_string_init(arguments.size);
    bit_string_writer_t *writer = bit_string_writer_init(file);

    srand(time(NULL));
    for(int i = 0; i < arguments.size * 8; i++) {
        if(rand() >= arguments.probability * RAND_MAX) {
            bit_string_append_bit(bs,0);
        } else {
            bit_string_append_bit(bs,1);
        }
    }
    bit_string_writer_write(writer,bs);
    bit_string_writer_flush(writer);
    
    bit_string_writer_destroy(writer);
    bit_string_destroy(bs);
    
    return 0;
}
