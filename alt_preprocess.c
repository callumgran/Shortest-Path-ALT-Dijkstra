#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "dijkstra.h"
#include "alt_preprocess.h"

int n_threads;
pthread_mutex_t t_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t t_cond = PTHREAD_COND_INITIALIZER;

static void write_distances_to_file(struct shortest_path *sp, struct dijkstra_t *d)
{
    FILE *output;
    output = fopen(d->name, "w");

    if (output == NULL) {
        fprintf(stderr, "could not open file :(");
        exit(1);
    }
        

    for (int i = 0; i < d->graph->node_count; i++)
        fwrite(&sp[i].total_cost, 4, 1, output);
    
    printf("done\n");
    fclose(output);
}

static void *dijkstra_thread(void *arg)
{
    struct dijkstra_t *d;

    d = (struct dijkstra_t *)arg;
    
    
    struct shortest_path *sp = dijkstra(d->graph, d->start_node);

    write_distances_to_file(sp, d);

    graph_free(d->graph);
    free(d->name);
    free(sp);
    free(arg);
    pthread_mutex_lock(&t_mutex);
    n_threads -= 1;
    if (n_threads == 0)
        pthread_cond_signal(&t_cond);
    pthread_mutex_unlock(&t_mutex);
}

static void create_thread(struct graph_t *graph, int start_node, char* name)
{
    pthread_t thread;
    struct dijkstra_t *d = (struct dijkstra_t *)
                                (malloc(sizeof(struct dijkstra_t)));
    d->graph = graph;
    d->start_node = start_node;
    d->name = name;

    pthread_mutex_lock(&t_mutex);
    n_threads += 1;
    pthread_mutex_unlock(&t_mutex);
    pthread_create(&thread, NULL, dijkstra_thread, (void *)d);
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
    strncpy(output_files[1], "2cor.txt", 32);
    strncpy(output_files[2], "2rev.txt", 32);
    strncpy(output_files[3], "2cor.txt", 32);
    strncpy(output_files[4], "3rev.txt", 32);
    strncpy(output_files[5], "3cor.txt", 32);
    
    n_threads = 0;
    int j = 0;
    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        struct graph_t graph_correct;
        struct graph_t graph_reversed;

        parse_node_file(node_file, &graph_correct);
        parse_edge_file(edge_file, &graph_correct, false);
        create_thread(&graph_correct, landmarks[i], output_files[j++]);

        parse_node_file(node_file, &graph_reversed);
        parse_edge_file(edge_file, &graph_reversed, true);
        create_thread(&graph_reversed, landmarks[i], output_files[j++]);
    }
    
    pthread_mutex_lock(&t_mutex);
    pthread_cond_wait(&t_cond, &t_mutex);
    pthread_mutex_unlock(&t_mutex);
}
