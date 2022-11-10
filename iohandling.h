#ifndef IOHANDLING_H
#define IOHANDLING_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Defines */
#define ERROR_EOF "-1"

/* Methods for file handling */

/* Method to get the collection of chars from input. */
char *f_next_item(FILE *fp);

/* Method to get the next int from a file. */
int f_next_int(FILE *fp);

/* Method to get the next double from a file. */
double f_next_double(FILE *fp);

#endif