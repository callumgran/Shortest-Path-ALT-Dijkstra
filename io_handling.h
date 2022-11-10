#ifndef IO_HANDLING_H
#define IO_HANDLING_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Defines */
#define ERROR_EOF -1

/* Methods for file handling */

/* Method to get the next int from a file. */
int f_next_int(FILE *fp);

/* Method to get the next double from a file. */
double f_next_double(FILE *fp);

/* Method to consume characters until, and including, the next '\n' */
void f_consume_line(FILE *fp);

#endif
