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

#define NUMBER_OF_LANDMARKS 5

/* Landmark node indexes */
#define LANDMARK_ONE 5436444
#define LANDMARK_TWO 2507642
#define LANDMARK_THREE 1235456
#define LANDMARK_FOUR 6857624
#define LANDMARK_FIVE 4417787

/* Structs */
struct thread_data_t {
    struct graph_t *graph;
    int* landmarks;
    char **names;
    bool reversed;
};

/* Methods */

/* Method to execute multithreaded preprocessing for
     distance to and from landmarks. */
void preprocess(char *node_file, char *edge_file);

#endif