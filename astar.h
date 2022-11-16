#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define INF (999999999)
#define NUMBER_OF_LANDMARKS 3

/* Method */
void do_astar(char *node_file, char *edge_file, int start_node, int end_node);