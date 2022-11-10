#ifndef IOHANDLING_H
#define IOHANDLING_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Defines */
#define ERROR_EOF -1

/* Methods for file handling */

/* Method to get the collection of chars from input. */
char *f_next_item(FILE *fp);

/* Method to get the next int from a file. */
int f_next_int(FILE *fp);

/* Method to get the next double from a file. */
double f_next_double(FILE *fp);

/* Method to consooooooooome entire line */
void f_consume_line(FILE *fp);


/* function definitions */

void f_consume_line(FILE *fp)
{
    char ch;
    while ((ch = fgetc(fp))) {
        if (ch == '\n')
            break;
        if (ch == EOF)
            break;
    }
}

char *f_next_item(FILE *fp)
{
    char    ch;
    char    *buf;
    int     i;

    buf = (char *)(malloc(sizeof(char) * 128));

    while ((ch = fgetc(fp)) == ' ');

    if (ch == EOF)
        return NULL;

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
        return NULL;
    
    return buf;
}

int f_next_int(FILE *fp)
{
    char        *buf;
    int         ret;
    
    buf = f_next_item(fp);
    if (buf)
        return ERROR_EOF;
    ret = atoi(buf);

    free(buf);
    return ret;
}

double f_next_double(FILE *fp)
{
    char        *buf;
    double      ret;
    
    buf = f_next_item(fp);
    if (buf == NULL)
        return ERROR_EOF;

    ret = strtod(buf, NULL);
    free(buf);

    return ret;
}

#endif
