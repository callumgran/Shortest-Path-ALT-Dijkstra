#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define INF (999999999)
#define NUMBER_OF_LANDMARKS 3
#define VISITED_START_CAP 100

/* Structs */
struct file_data_t {
    char *data;
    size_t data_len;
};

union byte_int_conv {
    int c;
    char bytes[4];
};

/* Method */
void do_astar(char *node_file, char *edge_file, int start_node, int end_node);