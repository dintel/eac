#ifndef LOG_H
#define LOG_H

#include <stdio.h>

/** Global flag that indicates whether verbose messages should be print */
extern int log_verbose;

/** Global flag that indicates whether debug messages should be print */
extern int log_debug;

/** Macro that outputs message only if log_verbose flag is set */
#define PRINT_VERBOSE(...) if(log_verbose) printf( __VA_ARGS__ );

/** Macro that outputs message only if log_debug flag is set */
#define PRINT_DEBUG(...) if(log_debug) printf( __VA_ARGS__ );

#endif
