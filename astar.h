#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define INF (1 << 30)
#define NODE_UNVISITED -1
#define NODE_START -2

/* Structs */
struct file_data_t {
    char *data;
    size_t data_len;
};

union byte_int_conv {
    int c;
    char bytes[4];
};