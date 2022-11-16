#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io_handling.h"

struct file_data_t *get_file_data(FILE *input)
{
    struct file_data_t *res = (struct file_data_t *)
                                (malloc(sizeof(struct file_data_t)));

    fseek(input, 0, SEEK_END);
    res->data_len = ftell(input);
    rewind(input);
    res->data = (char *)(malloc(res->data_len));
    size_t ret = fread(res->data, res->data_len, 1, input);

    if (ret != 1)
        return NULL;

    return res;
}

int** get_distance_list(FILE* input, char **file_names, int landmarks)
{
    int **arr = malloc(landmarks * sizeof(int *));

    union byte_int_conv byte_int_conv;

    for (int i = 0; i < landmarks; i++) {

        input = fopen(file_names[i], "rb");

        if (input == NULL) {
            fprintf(stderr, "could not open file: '%s'\n", file_names[i]);
            exit(1);
        }

        struct file_data_t *fd = get_file_data(input);

        fclose(input);
        
        arr[i] = (int *)(malloc(fd->data_len));

        int k = 0; int l = 0;
        for (int j = 0; j < fd->data_len; j++) {
            byte_int_conv.bytes[l++] = fd->data[j];
            if (l == 4) {
                arr[i][k++] = byte_int_conv.c;
                l = 0;
            }
        }

        free(fd->data);
        free(fd);
    }

    return arr;
}

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

char *f_next_str(FILE *fp)
{
    char    ch;
    char    buf[128];
    int     i = 0;

    while ((ch = fgetc(fp)) == ' ');

    if (ch == EOF)
        goto error_eof;

    ch = fgetc(fp);

    buf[i++] = ch;

    do {
        ch = fgetc(fp);
        if (ch == '"') {
            buf[i++] = 0;
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
    
    char *result = (char *)(malloc(i * sizeof(char)));
    for (int j = 0; j < i; j++)
        result[j] = buf[j];

    return result;

error_eof:
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

float f_next_float(FILE *fp)
{
    char        *buf;
    char        *strtofp;
    float      ret;
    
    buf = f_next_item(fp);
    if (buf == NULL)
        return ERROR_EOF;

    ret = atof(buf);
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
