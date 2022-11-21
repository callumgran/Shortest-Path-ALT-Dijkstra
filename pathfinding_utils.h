#ifndef PATHFINDING_UTILS_H
#define PATHFINDING_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include "heap_queue.h"
#include "graph.h"

/* Defines */
#define INF (999999999)
#define NUMBER_OF_LANDMARKS 6

/* Methods */

/* Compare method for the heap. */
bool compare(void *a, void *b);

/* Method to push a node to the heap. */
void heapq_push_node(struct heapq_t *hq, int node, int total_cost);

/* Method to initialize all previous nodes. */
void init_prev(struct graph_t *graph, int start);

/* Method to convert from centi-seconds and print as hours, minutes and seconds.*/
void print_centi_to_drive_time(int centi);

/* Method to print out the path the route took. */
void print_path(struct node_t *end_node, int start_node);

/* Method to write path to file with latitude and longitude.*/
void write_path_to_file(struct graph_t *graph, int end_node, int start_node);

/* Method to write specified points of interest around a node to a file.*/
void write_poi_to_file(struct graph_t *graph, int* arr, int nodes, char* out_file);

#endif