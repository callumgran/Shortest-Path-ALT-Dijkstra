#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "dijkstra.h"
#include "alt_preprocess.h"

int n_threads;
pthread_mutex_t t_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t t_cond = PTHREAD_COND_INITIALIZER;

static void *dijkstra_thread(void *arg, struct thread_info_t *t_info)
{
    void            **params;
    int             start_node;
    struct graph_t  *graph;

    params = (void **)arg;
    graph = (struct graph_t *)params[0];
    start_node = (int)params[1];
    
    // Run Dijkstra
    free(arg);
    pthread_mutex_lock(&t_mutex);
    n_threads -= 1;
    if (n_threads == 0)
        pthread_cond_signal(&t_cond);
    pthread_mutex_unlock(&t_mutex);
}

void preprocess(char *node_file, char *edge_file)
{
    int landmarks[3] = {LANDMARK_ONE, 
                        LANDMARK_TWO, 
                        LANDMARK_THREE};
    struct graph_t *graph_correct;
    struct graph_t *graph_reversed;
    void **param;
    pthread_t thread[START_THREADS];

    parse_node_file(node_file, graph_correct);
    parse_edge_file(edge_file, graph_correct, false);

    parse_node_file(node_file, graph_correct);
    parse_edge_file(edge_file, graph_reversed, true);

    
    n_threads = START_THREADS;

    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        param = (void **)(malloc(DIJKSTRA_PARAM_SIZE));
        allocate_thread_params(graph_correct, landmarks[i]);
        pthread_create(&thread[i], NULL, dijkstra_thread, param);
        param = (void **)(malloc(DIJKSTRA_PARAM_SIZE));
        allocate_thread_params(graph_reversed, landmarks[i]);
        pthread_create(&thread[i], NULL, dijkstra_thread, param);
    }
    
    pthread_mutex_lock(&t_mutex);
    pthread_cond_wait(&t_cond, &t_mutex);
    pthread_mutex_unlock(&t_mutex);
}
