#ifndef GRAPH_H
#define GRAPH_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "io_handling.h"

/* Structs */
struct node_t {
    int node_idx;
    float latitude;
    float longitude;
    struct edge_t *first_edge;
    struct prev_t *d;
    int code;
    char *name;
};

struct prev_t {
    int dist;
    struct node_t *prev;
};

struct node_info_t {
    int node_idx;
    int total_cost;
};

struct edge_t {
    struct edge_t *next_edge;
    struct node_t *to_node;
    int cost;
};

struct graph_t {
    struct node_t *n_list;
    int node_count;
    int edge_count;
};

/* Methods */

/* Method to print a graph. */
void graph_print(struct graph_t *graph);

/* Method to add a position of interest to a graph. */
void graph_insert_poi(struct graph_t *graph, int node_idx, int node_code, char* name);

/* Method to add a node to a graph. */
void graph_insert_node(struct graph_t *graph, int node_idx, float latitude, float longitude);

/* Method to add an edge to a graph */
void graph_insert_edge(struct graph_t *graph, int from_idx, int to_idx, int cost);

/* Method to free graph and all nodes and edges. */
void graph_free(struct graph_t *graph);

/* Method to parse nodes into graph from file. */
/* Return true if an error occurs. */
bool parse_node_file(char *file_name, struct graph_t *graph);

/* Method to parse edges into graph from file. */
/* Return true if an error occurs. */
bool parse_edge_file(char *file_name, struct graph_t *graph);

/* Method to parse points of interest into graph from file. */
/* Return true if an error occurs. */
bool parse_poi_file(char *file_name, struct graph_t *graph);

/* Method for transposing a graph. */
struct graph_t *graph_transpose(struct graph_t *graph);

/* Method for copying a graph. */
struct graph_t *graph_copy(struct graph_t *graph);

#endif