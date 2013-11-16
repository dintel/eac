#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <unistd.h>
#include "bit_string.h"
#include "bit_string_writer.h"
#include "log.h"

/** Generator version */
const char *argp_program_version = "eac_generator 0.1";
/** Email to send generator bug reports */
const char *argp_program_bug_address = "<dmitry.zbarski@gmail.com>";

/** Program documentation. */
static char doc[] = "Entropy Adaptive Coding - generator";

/** A description of the arguments we accept. */
static char args_doc[] = " -o OUTPUT_FILE -n SIZE";

/** The options encoder accpets */
static struct argp_option options[] = {
    {"size",   'n', "FILE",       0, "Size of file in bytes" },
    {"zero",   '0', "NUM",        0, "Probability of 0" },
    {"one",    '1', "NUM",        0, "Probability of 1" },
    {"output", 'o', "FILE",       0, "Output file name" },
    { 0 }
};
     
/** \brief Encoder arguments structure */
struct generator_arguments {
    size_t size;         /**< window size parameter */
    char *file;          /**< output file parameter */
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
     
    case ARGP_KEY_ARG:
        if(state->arg_num >= 1)
            argp_usage (state);
        break;
     
    case ARGP_KEY_END:
        if(arguments->output_file == 0)
            argp_usage (state);
        break;
     
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/** Encoder argp parser */
static struct argp argp = { options, generator_parse_opt, args_doc, doc };

int main(int argc, char *argv[])
{
    
    
    return 0;
}
