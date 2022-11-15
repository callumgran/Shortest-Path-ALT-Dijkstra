#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* Defines */
#define INF (999999999)

/* Methods */

/* Dijkstra methods */

/* Method to do dijkstra. */
/* Only runs until the end node is found.*/
/* Returns checked nodes. */
int dijkstra(struct graph_t *graph, int start_node, int end_node);

/* Dijkstra method for preprocessing. */
/* Finds all nodes. */
void dijkstra_pre_process(struct graph_t *graph, int start_node);

/* Initializing function to do dijkstra's algorithm with data. */
void do_dijkstra(char *node_file, char *edge_file, int starting_node, int end_node);

#endif