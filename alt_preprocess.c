#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "graph.h"
#include "dijkstra.h"
#include "alt_preprocess.h"

int n_threads;
pthread_mutex_t t_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t t_cond = PTHREAD_COND_INITIALIZER;

/* Method to write distances to a file that can be read when you run ALT. */
static void write_distances_to_file(struct graph_t *graph, 
                                    struct thread_data_t *ti, 
                                    int idx)
{
    FILE *output;
    output = fopen(ti->names[idx], "wb");

    if (output == NULL) {
        fprintf(stderr, "could not open file :(");
        exit(1);
    }

    for (int i = 0; i < graph->node_count; i++)
        fwrite(&graph->n_list[i].d->dist, 4, 1, output);

    fclose(output);
}

/* Method to write distances to a file that can be read when you run ALT. */
static void *dijkstra_thread(void *arg)
{
    struct thread_data_t *ti;
    struct graph_t graph;

    ti = (struct thread_data_t *)arg;

    if (ti->reversed)
        graph = *graph_transpose(ti->graph);
    else
        graph = *graph_copy(ti->graph);
    
    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        dijkstra_pre_process(&graph, ti->landmarks[i]);
        write_distances_to_file(&graph, ti, i);
    }

    graph_free(&graph);
    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        free(ti->names[i]);
    }
    free(ti->names);
    free(arg);
    pthread_mutex_lock(&t_mutex);
    n_threads -= 1;
    if (n_threads == 0)
        pthread_cond_signal(&t_cond);
    pthread_mutex_unlock(&t_mutex);
}

static void create_thread(struct graph_t *graph, int *landmarks, 
                                        char** names, bool reversed)
{
    pthread_t thread;
    struct thread_data_t *ti = (struct thread_data_t *)
                                (malloc(sizeof(struct thread_data_t)));
    ti->graph = graph;
    ti->landmarks = landmarks;
    ti->names = names;
    ti->reversed = reversed;

    pthread_mutex_lock(&t_mutex);
    n_threads++;
    pthread_mutex_unlock(&t_mutex);
    pthread_create(&thread, NULL, dijkstra_thread, (void *)ti);
}

void preprocess(char *node_file, char *edge_file)
{
    int landmarks[5] = {LANDMARK_ONE, 
                        LANDMARK_TWO, 
                        LANDMARK_THREE,
                        LANDMARK_FOUR, 
                        LANDMARK_FIVE};


    char **output_files_cor = malloc(sizeof(char *) * NUMBER_OF_LANDMARKS);
    char **output_files_rev = malloc(sizeof(char *) * NUMBER_OF_LANDMARKS);
    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        output_files_cor[i] = malloc(sizeof(char) * 32);
        output_files_rev[i] = malloc(sizeof(char) * 32);
    }
    
    int j = 0;
    strncpy(output_files_rev[j], "1rev.txt", 32);
    strncpy(output_files_cor[j++], "1cor.txt", 32);
    strncpy(output_files_rev[j], "2rev.txt", 32);
    strncpy(output_files_cor[j++], "2cor.txt", 32);
    strncpy(output_files_rev[j], "3rev.txt", 32);
    strncpy(output_files_cor[j++], "3cor.txt", 32);
    strncpy(output_files_rev[j], "4rev.txt", 32);
    strncpy(output_files_cor[j++], "4cor.txt", 32);
    strncpy(output_files_rev[j], "5rev.txt", 32);
    strncpy(output_files_cor[j++], "5cor.txt", 32);
    
    n_threads = 0;

    struct graph_t graph;
    double elapsed;
    struct timespec start, finish;

    clock_gettime(CLOCK_MONOTONIC, &start);
    parse_node_file(node_file, &graph);
    parse_edge_file(edge_file, &graph);

    create_thread(&graph, landmarks, output_files_rev, true);
    create_thread(&graph, landmarks, output_files_cor, false);
    
    pthread_mutex_lock(&t_mutex);
    pthread_cond_wait(&t_cond, &t_mutex);
    pthread_mutex_unlock(&t_mutex);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Time used for preprocessing: %f s.\n", elapsed);

    graph_free(&graph);
}