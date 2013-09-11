#ifndef LOG_H
#define LOG_H

#include <stdio.h>

extern int log_verbose;
extern int log_debug;

#define PRINT_VERBOSE(...) if(log_verbose) printf( __VA_ARGS__ );
#define PRINT_DEBUG(...) if(log_debug) printf( __VA_ARGS__ );

#endif
