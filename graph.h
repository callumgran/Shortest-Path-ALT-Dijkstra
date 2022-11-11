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
    long latitude;
    long longitude;
};

struct node_info_t {
    int node_idx;
    int cost_from_start_node;
};

struct edge_t {
    /* index/value/identifer of the node */
    int from_idx;
    int to_idx;
    int cost;
    struct edge_t *next;
};

struct graph_t {
    struct node_t **node_list;
    /*
     * every node in the node_list has a linked list of edges.
     * the value at the index of the neighbour_list is the head of the linked list of edges.
     *
     * example:
     * [0]      -> edge -> edge -> edge
     * [1]      -> edge
     * [2]      -> NULL
     * [3]      -> edge -> edge
     * ...
     * [i]
     */
    struct edge_t **neighbour_list;
    int node_count;
    int edge_count;
};

/* Methods */

/* Method to print a graph. */
void graph_print(struct graph_t *graph);

/* Method to add a node to a graph. */
void graph_insert_node(struct graph_t *graph, int node_idx, double latitude, double longitude);

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

/* Method for transposing a graph. */
struct graph_t *graph_transpose(struct graph_t *graph);

/* Method for copying a graph. */
struct graph_t *graph_copy(struct graph_t *graph);

#endif