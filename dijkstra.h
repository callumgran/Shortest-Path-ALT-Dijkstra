#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* Defines */
#define INF (1 << 30)
#define NODE_UNVISITED -1
#define NODE_START -2

/* Structs */

/* ad-hoc datastructure to store information given by performing dijkstra */
struct shortest_path {
    int previous_idx;
    int total_cost;
};

/* Methods */

/* Dijkstra method */
/* Finds the shortest path from a to all other nodes in a graph from a start node. */
struct shortest_path *dijkstra(struct graph_t *graph, int start_node);

/* Print method to show shortest path to each node in the graph. */
void shortest_path_print(struct shortest_path *sp, int node_count);

/* Initializing function to do dijkstra's algorithm with data. */
void do_dijkstra(char *node_file, char *edge_file, int starting_node);

#endif