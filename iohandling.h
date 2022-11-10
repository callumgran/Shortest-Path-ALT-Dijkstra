#ifndef IOHANDLING_H
#define IOHANDLING_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Defines */
#define ERROR_EOF "-1"

/* Methods for file handling */

/* Method to get the collection of chars from input. */
static char *f_next_item(FILE *fp);

/* Method to get the next int from a file. */
static int f_next_int(FILE *fp);

/* Method to get the next double from a file. */
static double f_next_double(FILE *fp);

static char *f_next_item(FILE *fp)
{
    char    ch;
    char    *buf;
    int     i;

    buf = (char *)(malloc(sizeof(char) * 128));

    while ((ch = fgetc(fp)) == ' ') {
    }

    if (ch == EOF)
        return ERROR_EOF;

    buf[i++] = ch;

    do {
        ch = fgetc(fp);
        if (ch == ' ') {
            buf[i] = 0;
            break;
        } else if (ch == '\n') {
            break;
        } else if (i >= 128) {
            fprintf(stderr, "Error parsing file: node longer than 128 chars\n");
            exit(1);
            break;
        }

        buf[i++] = ch;
    } while (ch != EOF);

    if (ch == EOF)
        return ERROR_EOF;
    
    return buf;
}

static int f_next_int(FILE *fp)
{
    char        *buf;
    int         ret;
    
    buf = f_next_item(fp);
    ret = atoi(buf);

    free(buf);
    return ret;
}

static double f_next_double(FILE *fp)
{
    char        *buf;
    char        *strtodp;
    double      ret;
    
    buf = f_next_item(fp);
    ret = strtod(buf, &strtodp);

    free(buf);
    return ret;
}

#endif