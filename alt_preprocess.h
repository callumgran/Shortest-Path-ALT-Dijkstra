#ifndef ALT_PREPROCESS_H
#define ALT_PREPROCESS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "dijkstra.h"

/* Defines */

#define POINTER_SIZE sizeof(char *)
#define DIJKSTRA_PARAM_SIZE POINTER_SIZE << 1
#define NUMBER_OF_LANDMARKS 3
#define START_THREADS NUMBER_OF_LANDMARKS << 1
#define LANDMARK_ONE 1
#define LANDMARK_TWO 1
#define LANDMARK_THREE 1
#define allocate_thread_params(graph, node)     \
    do {                                        \
        param[0] = graph;                       \
        param[1] = node;                        \
    } while(0);                                 \


/* Methods */

/* Method to preprocess distance to and from landmarks. */
void preprocess(char *node_file, char *edge_file);


#endif