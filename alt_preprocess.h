#ifndef ALT_PREPROCESS_H
#define ALT_PREPROCESS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "graph.h"
#include "dijkstra.h"

/* Defines */

#define NUMBER_OF_LANDMARKS 3
#define START_THREADS NUMBER_OF_LANDMARKS << 1

/* Landmark node indexes */
#define LANDMARK_ONE 11111
#define LANDMARK_TWO 55555
#define LANDMARK_THREE 77777

/* Structs */
struct thread_data_t {
    struct graph_t *graph;
    int start_node;
    char *name;
    bool reversed;
};

/* Methods */

/* Method to execute multithreaded preprocessing for
     distance to and from landmarks. */
void preprocess(char *node_file, char *edge_file);

#endif