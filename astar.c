#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "heap_queue.h"
#include "graph.h"
#include "io_handling.h"
#include "astar.h"

static void heapq_push_node(struct heapq_t *hq, int node, int total_cost)
{
    struct node_info_t *ni = malloc(sizeof(struct node_info_t));
    ni->node_idx = node;
    ni->total_cost = total_cost;
    heapq_push(hq, ni);
}

static bool compare(void *a, void *b)
{
    return ((struct node_info_t *)a)->total_cost > ((struct node_info_t *)b)->total_cost;
}

static struct prev_t *malloc_prev(int i)
{
    struct prev_t *prev = (struct prev_t *)(malloc(sizeof(struct prev_t)));
    prev->dist = INF;
    return prev;
}

static void init_prev(struct graph_t *graph, int start){
    for (int i = 0; i < graph->node_count; i++) {
        graph->n_list[i].d = malloc_prev(i);
    }
    graph->n_list[start].d->dist = 0;
}

/* Estimated distance functions */

/* Method that returns 0 or the the distance from the landmark to the end 
    minus the distance from the landmark to the current node. */
static inline int est_dist_l_e(int **from_list, int i, int end_idx, int node_idx)
{
    int dist = from_list[i][end_idx] - from_list[i][node_idx];
    return dist > 0 ? dist : 0;
}

/* Method that returns 0 or the the distance from the current node to the landmark 
    minus the distance from the end to the landmark. */
static inline int est_dist_e_l(int **to_list, int i, int end_idx, int node_idx)
{
    int dist = to_list[i][node_idx] - to_list[i][end_idx];
    return dist > 0 ? dist : 0;
}

/* Method that returns the highest estimate found for the distance via triangulation. */
static int est_dist(int **from_list, int **to_list, int end_idx, int node_idx)
{   
    int est_dist = 0;
    int tmp_est = 0;
    
    for (int i = 0; i < 3; i++) {
        tmp_est = est_dist_l_e(from_list, i, end_idx, node_idx);
        if (tmp_est > est_dist)
            est_dist = tmp_est;
        tmp_est = est_dist_e_l(to_list, i, end_idx, node_idx);
        if (tmp_est > est_dist)
            est_dist = tmp_est;
    }

    return est_dist;
}

/* A-star implementation */
int astar(struct graph_t *graph, int **from_list, int **to_list, int start_node, int end_node)
{
    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);
    struct heapq_t *hq = heapq_malloc(compare);
    init_prev(graph, start_node);

    heapq_push_node(hq, start_node, 0 + est_dist(from_list, to_list, end_node, start_node));

    struct node_t *curr;
    struct node_info_t *ni;
    int nodes_checked = 0;

    while ((ni = (struct node_info_t *)heapq_pop(hq))) {
        nodes_checked++;
        visited[ni->node_idx] = true;

        if (visited[end_node]) {
            free(ni);
            break;
        }

        curr = &graph->n_list[ni->node_idx];

        for (struct edge_t *edge = curr->first_edge; edge; edge = edge->next_edge) {

            if (visited[edge->to_node->node_idx])
                continue;

            int new_cost = curr->d->dist + edge->cost;
            if (edge->to_node->d->dist > new_cost) {
                edge->to_node->d->dist = new_cost;
                edge->to_node->d->prev = curr;
                heapq_push_node(hq, edge->to_node->node_idx, (new_cost + est_dist(from_list, to_list, 
                                                                end_node, edge->to_node->node_idx)));
            }

        }
        /* 
         * why free? heapq_pop() returns a malloced node that would otherwise be lost if it was not
         * freed here 
         */
        free(ni);
    }

    heapq_free(hq);

    return nodes_checked;
}

void do_astar(char *node_file, char *edge_file, int start_node, int end_node)
{
    FILE *input;
    clock_t t;
    char **from_files = malloc(sizeof(char *) * 6);
    char **to_files = malloc(sizeof(char *) * 6);
    for (int i = 0; i < 3; i++) {
        from_files[i] = malloc(sizeof(char) * 32);
        to_files[i] = malloc(sizeof(char) * 32);
    }

    strncpy(to_files[0], "1rev.txt", 32);
    strncpy(to_files[1], "2rev.txt", 32);
    strncpy(to_files[2], "3rev.txt", 32);
    strncpy(from_files[0], "1cor.txt", 32);
    strncpy(from_files[1], "2cor.txt", 32);
    strncpy(from_files[2], "3cor.txt", 32);
    
    struct graph_t graph;

    parse_node_file(node_file, &graph);
    parse_edge_file(edge_file, &graph);

    int **to_list = malloc(NUMBER_OF_LANDMARKS * sizeof(int *));
    int **from_list = malloc(NUMBER_OF_LANDMARKS * sizeof(int *));

    get_distance_list(input, to_files, to_list, NUMBER_OF_LANDMARKS);
    get_distance_list(input, from_files, from_list, NUMBER_OF_LANDMARKS);

    free(from_files);
    free(to_files);

    t = clock();
    int nodes_checked = astar(&graph, from_list, to_list, start_node, end_node);
    t = clock() - t;
    double time_taken = ((double)t)/(CLOCKS_PER_SEC/1000);

    printf("Nodes checked: %d \n", nodes_checked);
    printf("Driving time in centi-seconds: %d\n", graph.n_list[end_node].d->dist);
    int hours = (graph.n_list[end_node].d->dist) / 360000;
    int mins = (graph.n_list[end_node].d->dist - (hours * 360000))/6000;
    int secs = (graph.n_list[end_node].d->dist - (hours * 360000) - (mins * 6000))/100;
    
    printf("Driving time: %d:%d:%d\n", hours, mins, secs);

    struct node_t *node = &graph.n_list[end_node];
    while (node->node_idx != start_node) {
        printf("%d <- ", node->node_idx);
        node = node->d->prev;
    }

    printf("%d", start_node);
    printf("\n");
    printf("Time used for ALT: %f ms \n", time_taken);

    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        free(to_list[i]);
        free(from_list[i]);
    }
    
    free(from_list);
    free(to_list);
}