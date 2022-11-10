#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io_handling.h"

static char *f_next_item(FILE *fp)
{
    char    ch;
    char    *buf;
    int     i = 0;

    buf = (char *)(malloc(sizeof(char) * 128));

    while ((ch = fgetc(fp)) == ' ');

    if (ch == EOF)
        goto error_eof;

    buf[i++] = ch;

    do {
        ch = fgetc(fp);
        if (ch == ' ' || ch == '\t') {
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
        goto error_eof;
    
    return buf;

error_eof:
    free(buf);
    return NULL;
}

int f_next_int(FILE *fp)
{
    char        *buf;
    int         ret;
    
    buf = f_next_item(fp);
    if (buf == NULL)
        return ERROR_EOF;

    ret = atoi(buf);
    free(buf);
    return ret;
}

double f_next_double(FILE *fp)
{
    char        *buf;
    char        *strtodp;
    double      ret;
    
    buf = f_next_item(fp);
    if (buf == NULL)
        return ERROR_EOF;

    ret = strtod(buf, &strtodp);
    free(buf);
    return ret;
}

void f_consume_line(FILE *fp)
{
    char ch;
    /* consume entire line */
    while ((ch = fgetc(fp))) {
        if (ch == '\n')
            break;
        if (ch == EOF)
            break;
    }
}
