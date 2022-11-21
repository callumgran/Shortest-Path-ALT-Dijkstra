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

#define NUMBER_OF_LANDMARKS 6

/* Landmark node indexes */
#define LANDMARK_ONE 918769 //Karlskrona
#define LANDMARK_TWO 894067 //Helsinki
#define LANDMARK_THREE 5770561 //Bergen
#define LANDMARK_FOUR 2438190 //Fauske
#define LANDMARK_FIVE 412001 //Kuusamo
#define LANDMARK_SIX 5436444 //Karasjok

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