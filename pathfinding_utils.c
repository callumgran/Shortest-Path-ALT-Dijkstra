#include <stdlib.h>
#include <stdio.h>
#include "pathfinding_utils.h"
#include "heap_queue.h"
#include "graph.h"

bool compare(void *a, void *b)
{
    return ((struct node_info_t *)a)->total_cost > ((struct node_info_t *)b)->total_cost;
}

void heapq_push_node(struct heapq_t *hq, int node, int total_cost)
{
    struct node_info_t *ni = malloc(sizeof(struct node_info_t));
    ni->node_idx = node;
    ni->total_cost = total_cost;
    heapq_push(hq, ni);
}

void init_prev(struct graph_t *graph, int start)
{
    for (int i = 0; i < graph->node_count; i++) {
        (graph->n_list + i)->d->dist = INF;
        (graph->n_list + i)->d->prev = NULL;
    }
    (graph->n_list + start)->d->dist = 0;
}

void print_centi_to_drive_time(int centi)
{
    int hours = (centi) / 360000;
    int mins = (centi - (hours * 360000))/6000;
    int secs = (centi - (hours * 360000) - (mins * 6000))/100;
    
    printf("Driving time: %d:%d:%d\n", hours, mins, secs);
}

void print_path(struct node_t *end_node, int start_node)
{
    struct node_t *node = end_node;
    int j = 0;
    while (node->node_idx != start_node) {
        j++;
        printf("%d <- ", node->node_idx);
        node = node->d->prev;
        if (j == 5) {
            putchar('\n');
            j = 0;
        }
    }
    printf("%d\n\n", start_node);
}

void write_path_to_file(struct graph_t *graph, int end_node, int start_node)
{
    FILE* fp;

    fp = fopen("path.csv", "w");

    struct node_t *node = (graph->n_list + end_node);
    while (node->node_idx != start_node) {
        fprintf(fp, "%.7f, %.7f \n", node->latitude, node->longitude);
        node = node->d->prev;
    }
    fprintf(fp, "%.7f, %.7f \n", node->latitude, node->longitude);

    fclose(fp);

    printf("Path printed to path.csv \n");
}

void write_poi_to_file(struct graph_t *graph, int* arr, int nodes, char* out_file)
{
    FILE* fp;

    fp = fopen(out_file, "w");

    struct node_t *node;
    for (int i = 0; i < nodes; i++) {
        node = (graph->n_list + *(arr + i));
        fprintf(fp, "%.7f, %.7f \n", node->latitude, node->longitude);
    }

    fclose(fp);

    printf("POI's printed to %s \n", out_file);
}