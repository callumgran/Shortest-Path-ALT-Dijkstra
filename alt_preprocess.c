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

static void write_distances_to_file(struct shortest_path *sp, 
                            struct thread_info_t *ti, int node_count)
{
    FILE *output;
    output = fopen(ti->name, "w");

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
    struct thread_info_t *ti;
    struct graph_t graph;

    ti = (struct thread_info_t *)arg;

    parse_node_file(ti->node_file, &graph);
    parse_edge_file(ti->edge_file, &graph, ti->reversed);
    
    
    struct shortest_path *sp = dijkstra(&graph, ti->start_node);

    write_distances_to_file(sp, ti, graph.node_count);

    graph_free(&graph);
    free(ti->name);
    free(sp);
    free(arg);
    pthread_mutex_lock(&t_mutex);
    n_threads -= 1;
    if (n_threads == 0)
        pthread_cond_signal(&t_cond);
    pthread_mutex_unlock(&t_mutex);
}

static void create_thread(char *node_file, char *edge_file, 
                            int start_node, char* name, bool reversed)
{
    pthread_t thread;
    struct thread_info_t *ti = (struct thread_info_t *)
                                (malloc(sizeof(struct thread_info_t)));
    ti->node_file = node_file;
    ti->edge_file = edge_file;
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
    int j = 0;

    create_thread(node_file, edge_file, landmarks[0], output_files[j++], true);
    create_thread(node_file, edge_file, landmarks[0], output_files[j++], false);
    create_thread(node_file, edge_file, landmarks[1], output_files[j++], true);
    create_thread(node_file, edge_file, landmarks[1], output_files[j++], false);
    create_thread(node_file, edge_file, landmarks[2], output_files[j++], true);
    create_thread(node_file, edge_file, landmarks[2], output_files[j++], false);
    
    pthread_mutex_lock(&t_mutex);
    pthread_cond_wait(&t_cond, &t_mutex);
    pthread_mutex_unlock(&t_mutex);
}
