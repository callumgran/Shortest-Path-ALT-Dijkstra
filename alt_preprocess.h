#ifndef ALT_PREPROCESS_H
#define ALT_PREPROCESS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "dijkstra.h"

/* Defines */

#define NUMBER_OF_LANDMARKS 3
#define START_THREADS NUMBER_OF_LANDMARKS << 1
#define LANDMARK_ONE 1
#define LANDMARK_TWO 1
#define LANDMARK_THREE 1


/* Methods */

/* Method to execute multithreaded preprocessing for
     distance to and from landmarks. */
void preprocess(char *node_file, char *edge_file);

#endif