#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <argp.h>
#include <unistd.h>
#include <math.h>
#include "bit_string.h"
#include "bit_string_writer.h"
#include "lz77.h"
#include "log.h"
#include "queue.h"
#include "block.h"
#include "delta.h"

#define DEFAULT_BLOCK_SIZE 32768
#define DEFAULT_WINDOW_SIZE 32768
#define DEFAULT_THREADS 8

/** Encoder version */
const char *argp_program_version = "eac_encode 0.2";
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
    {"threads",     't', "THREADS",    0,                    "Number of concurrent threads"},
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
    int threads;                /**< number of threads */
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
        arguments->window_size = atoi(arg) * 8;
        break;
    case 'e':
        arguments->eac = 1;
        break;
    case 'b':
        arguments->block_size = atoi(arg);
        break;
    case 't':
        arguments->threads = atoi(arg);
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
    arguments.threads = DEFAULT_THREADS;
    
    /* Parse our arguments; every option seen by enc_parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    log_verbose = arguments.verbose;
    log_debug = arguments.debug;
    
    PRINT_DEBUG("INPUT_FILE = %s\nOUTPUT_FILE = %s\nWINDOW_SIZE = %zu bits\nBLOCK_SIZE = %d bytes\nEAC = %s\nVERBOSE = %s\nDEBUG = %s\n",
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
    
    uint8_t *buffer;
    bit_string_t *buffer_string;
    int buffer_size;
    long file_size = 0, compressed_size = 0;
    size_t *longest_match, longest_match_size, max_longest_match = 0;
    bit_string_writer_t *writer = bit_string_writer_init(outfile);

    if(arguments.eac) {
        queue_t *queue = queue_init();
        block_t *first = NULL;
        block_t *block;
        block_t *prev_block = NULL;
        int num = 0;

        buffer = malloc(sizeof(uint8_t) * arguments.block_size);
        longest_match_size = 0;
        
        while(!feof(file)) {
            buffer_size = fread(buffer,sizeof(uint8_t),arguments.block_size,file);
            if(buffer_size == 0) {
                continue;
            }
            buffer_string = convert(buffer,buffer_size);
            block = block_init(buffer_string,prev_block,num++);
            if(first == NULL)
                first = block;
            for(int i = MIN_NW; i <= MAX_NW && i <= arguments.block_size * 8; i *= 2 ) {
                queue_add_job(queue,block,i);
            }
            prev_block = block;
            longest_match_size++;
        }
        longest_match = malloc(sizeof(size_t) * longest_match_size);
        if(longest_match == NULL) {
            printf("Failed allocating memory for longest_match\n");
            return EXIT_FAILURE;
        }
        queue_run(queue,arguments.threads);
        block = first;
        while(block != NULL) {
            longest_match[block->num_block] = block->longest_match;
            if(max_longest_match < block->longest_match) {
                max_longest_match = block->longest_match;
            }
            if(block->prev_block != NULL) {
                uint8_t delta = nw_change_encode(block->prev_block->best_window_size,block->best_window_size);
                bit_string_writer_write_byte(writer,delta,NW_BITS);
                compressed_size += NW_BITS;
            }
            PRINT_VERBOSE("Block %d - best window size %zu compressed size %zu ratio %f\n", block->num_block, block->best_window_size, block->result->offset, (float)block->block->offset / block->result->offset);
            bit_string_writer_write(writer,block->result);
            file_size += block->block->offset;
            compressed_size += block->result->offset;
            block = block->next_block;
        }
        block = first;
        while(block != NULL) {
            block_t *tmp = block->next_block;
            block_destroy(block);
            block = tmp;
        }
        queue_destroy(queue);
    } else {
        bit_string_t *lz77_string;
        longest_match_size = 1;
        longest_match = malloc(sizeof(size_t));
        if(longest_match == NULL) {
            printf("Failed allocating memory for longest_match\n");
            return EXIT_FAILURE;
        }
        fseek(file,0L,SEEK_END);
        file_size = ftell(file);
        fseek(file,0L,SEEK_SET);
        buffer = malloc(sizeof(uint8_t) * file_size);
        buffer_size = fread(buffer,sizeof(uint8_t),file_size,file);
        buffer_string = convert(buffer,buffer_size);
        lz77_string = lz77_encode(buffer_string,arguments.window_size,NULL,longest_match);
        max_longest_match = *longest_match;
        PRINT_VERBOSE("Compressed using window size %zu compressed size %zu ratio %f\n", arguments.window_size, lz77_string->offset, (float)buffer_string->offset / lz77_string->offset);
        compressed_size += lz77_string->offset;
        file_size = buffer_string->offset;
        bit_string_writer_write(writer,lz77_string);
        bit_string_destroy(lz77_string);
    }
    free(buffer);

    if(compressed_size == 0) {
        fclose(file);
        fclose(outfile);
        unlink(arguments.output_file);
        printf("File too small to be compressed\n");
        return EXIT_FAILURE;
    }

    long avg_longest_match = 0;
    for(int i=0; i < longest_match_size; ++i) {
        avg_longest_match += longest_match[i];
    }
    avg_longest_match /= longest_match_size;

    long std_dev_longest_match = 0;
    for(int i=0; i < longest_match_size; ++i) {
        std_dev_longest_match += (avg_longest_match - longest_match[i]) * (avg_longest_match - longest_match[i]);
    }
    std_dev_longest_match /= longest_match_size;
    std_dev_longest_match = sqrt(std_dev_longest_match);

    compressed_size += bit_string_writer_flush(writer);
    printf("%ld;%ld;%f;%zu;%ld;%ld;",file_size,compressed_size, (double)file_size / compressed_size, max_longest_match,avg_longest_match,std_dev_longest_match);

    for(int i=0; i < longest_match_size; ++i) {
        avg_longest_match += longest_match[i];
        printf("%zu ",longest_match[i]);
    }

    bit_string_writer_destroy(writer);

    fclose(file);
    fclose(outfile);
    return EXIT_SUCCESS;
}
