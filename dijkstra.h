#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* Methods */

/* Dijkstra methods */

/* Dijkstra method for preprocessing. */
/* Finds all nodes. */
void dijkstra_pre_process(struct graph_t *graph, int start_node);

/* Initializing function to do dijkstra's algorithm for points of interest with data. */
void do_dijkstra_poi(char *node_file, char *edge_file, char* poi_file, 
                        int start_node, int node_type, char* out_file);
                        
/* Initializing function to do dijkstra's algorithm with data. */
void do_dijkstra(char *node_file, char *edge_file, int starting_node, int end_node);

#endif