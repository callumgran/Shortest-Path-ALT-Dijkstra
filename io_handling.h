#ifndef IO_HANDLING_H
#define IO_HANDLING_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Defines */
#define ERROR_EOF -1

/* Structs */
struct file_data_t {
    char *data;
    size_t data_len;
};

union byte_int_conv {
    int c;
    char bytes[4];
};

/* Methods for file handling */

/* Method to get the next int from a file. */
int f_next_int(FILE *fp);

/* Method to get the next double from a file. */
float f_next_float(FILE *fp);

/* Method to get the next string from a file. */
char *f_next_str(FILE *fp);

/* Method to consume characters until, and including, the next '\n' */
void f_consume_line(FILE *fp);

/* Method to return a distance list for ALT search. */
int** get_distance_list(FILE* input, char **file_names, int landmarks);

/* Method to get data from a file. */
struct file_data_t *get_file_data(FILE *input);

#endif
