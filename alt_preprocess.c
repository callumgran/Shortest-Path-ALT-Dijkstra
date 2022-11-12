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

static void write_distances_to_file(struct shortest_path *sp, 
                                    struct thread_data_t *ti, 
                                    int node_count, 
                                    int start_node)
{
    FILE *output;
    output = fopen(ti->name, "wb");

    if (output == NULL) {
        fprintf(stderr, "could not open file :(");
        exit(1);
    }

    for (int i = 0; i < node_count; i++)
        fwrite(&sp[i].total_cost, 4, 1, output);
    
    fclose(output);
}

static void *dijkstra_thread(void *arg)
{
    struct thread_data_t *ti;
    struct graph_t graph;
    double elapsed;

    ti = (struct thread_data_t *)arg;

    if (ti->reversed)
        graph = *graph_transpose(ti->graph);
    else
        graph = *graph_copy(ti->graph);

    struct shortest_path *sp = dijkstra(&graph, ti->start_node);

    write_distances_to_file(sp, ti, graph.node_count, ti->start_node);

    free(sp);
    graph_free(&graph);
    free(ti->name);
    free(arg);
    pthread_mutex_lock(&t_mutex);
    n_threads -= 1;
    if (n_threads == 0)
        pthread_cond_signal(&t_cond);
    pthread_mutex_unlock(&t_mutex);
}

static void create_thread(struct graph_t *graph, int start_node, 
                                        char* name, bool reversed)
{
    pthread_t thread;
    struct thread_data_t *ti = (struct thread_data_t *)
                                (malloc(sizeof(struct thread_data_t)));
    ti->graph = graph;
    ti->start_node = start_node;
    ti->name = name;
    ti->reversed = reversed;

    pthread_mutex_lock(&t_mutex);
    n_threads++;
    pthread_mutex_unlock(&t_mutex);
    pthread_create(&thread, NULL, dijkstra_thread, (void *)ti);
}

void preprocess(char *node_file, char *edge_file)
{
    int landmarks[3] = {LANDMARK_ONE, 
                        LANDMARK_TWO, 
                        LANDMARK_THREE};


    char **output_files = malloc(sizeof(char *) * 6);
    for (int i = 0; i < 6; i++)
        output_files[i] = malloc(sizeof(char) * 32);

    strncpy(output_files[0], "1rev.txt", 32);
    strncpy(output_files[1], "1cor.txt", 32);
    strncpy(output_files[2], "2rev.txt", 32);
    strncpy(output_files[3], "2cor.txt", 32);
    strncpy(output_files[4], "3rev.txt", 32);
    strncpy(output_files[5], "3cor.txt", 32);
    
    n_threads = 0;

    struct graph_t graph;
    double elapsed;
    struct timespec start, finish;

    clock_gettime(CLOCK_MONOTONIC, &start);
    parse_node_file(node_file, &graph);
    parse_edge_file(edge_file, &graph);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Time used for files was %f s.\n", elapsed);

    int j = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    create_thread(&graph, landmarks[0], output_files[j++], true);
    create_thread(&graph, landmarks[0], output_files[j++], false);
    create_thread(&graph, landmarks[1], output_files[j++], true);
    create_thread(&graph, landmarks[1], output_files[j++], false);
    create_thread(&graph, landmarks[2], output_files[j++], true);
    create_thread(&graph, landmarks[2], output_files[j++], false);
    
    pthread_mutex_lock(&t_mutex);
    pthread_cond_wait(&t_cond, &t_mutex);
    pthread_mutex_unlock(&t_mutex);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Time used for the rest was %f s.\n", elapsed);
    graph_free(&graph);
}