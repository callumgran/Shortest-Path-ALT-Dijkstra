#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "heap_queue.h"
#include "graph.h"
#include "io_handling.h"
#include "pathfinding_utils.h"
#include "alt.h"

/* Estimated distance functions */

/* Method that returns 0 or the the distance from the landmark to the end 
    minus the distance from the landmark to the current node. */
static inline int est_dist_l_e(int **from_list, int i, int end_idx, int node_idx)
{
    int dist = *(*(from_list + i) + end_idx) - *(*(from_list + i) + node_idx);
    return dist > 0 ? dist : 0;
}

/* Method that returns 0 or the the distance from the current node to the landmark 
    minus the distance from the end to the landmark. */
static inline int est_dist_e_l(int **to_list, int i, int end_idx, int node_idx)
{
    int dist = *(*(to_list + i) + node_idx) - *(*(to_list + i) + end_idx);
    return dist > 0 ? dist : 0;
}

/* Method that returns the highest estimate found for the distance via triangulation. */
static int est_dist(int **from_list, int **to_list, int end_idx, int node_idx)
{   
    int est_dist = 0;
    int tmp_est = 0;
    
    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
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
static int alt(struct graph_t *graph, int **from_list, int **to_list, int start_node, int end_node)
{
    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);

    struct heapq_t *hq = heapq_malloc(compare);
    init_prev(graph, start_node);

    heapq_push_node(hq, start_node, 0 + est_dist(from_list, to_list, end_node, start_node));

    struct node_info_t *ni;
    int nodes_checked = 0;
    while ((ni = (struct node_info_t *)heapq_pop(hq))->node_idx != end_node) {
        nodes_checked++;
        *(visited + ni->node_idx) = true;

        for (struct edge_t *edge = (graph->n_list + ni->node_idx)->first_edge; edge; edge = edge->next_edge) {
            if (!(*(visited + edge->to_node->node_idx))) {
                int new_cost = (graph->n_list + ni->node_idx)->d->dist + edge->cost;
                if (edge->to_node->d->dist > new_cost) {
                    edge->to_node->d->dist = new_cost;
                    edge->to_node->d->prev = (graph->n_list + ni->node_idx);
                    heapq_push_node(hq, edge->to_node->node_idx, (new_cost + est_dist(from_list, to_list, 
                                                                    end_node, edge->to_node->node_idx)));
                }
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


/* Method to get the files containing distances from
   landmarks to nodes. */
static char** get_from_files()
{
    char **from_files = malloc(sizeof(char *) * 8);

    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        *(from_files + i) = malloc(sizeof(char) * 32);
    }

    strncpy(*(from_files), "1cor.txt", 32);
    strncpy(*(from_files + 1), "2cor.txt", 32);
    strncpy(*(from_files + 2), "3cor.txt", 32);
    strncpy(*(from_files + 3), "4cor.txt", 32);
    strncpy(*(from_files + 4), "5cor.txt", 32);
    return from_files;
}

/* Method to get the files containing distances from
   nodes to landmarks. */
static char** get_to_files()
{
    char **to_files = malloc(sizeof(char *) * 8);

    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        *(to_files + i) = malloc(sizeof(char) * 32);
    }

    strncpy(*(to_files), "1rev.txt", 32);
    strncpy(*(to_files + 1), "2rev.txt", 32);
    strncpy(*(to_files + 2), "3rev.txt", 32);
    strncpy(*(to_files + 3), "4rev.txt", 32);
    strncpy(*(to_files + 4), "5rev.txt", 32);
    return to_files;
}

void do_alt(char *node_file, char *edge_file, int start_node, int end_node)
{
    FILE *input;
    clock_t t;
    bool err;
    struct graph_t graph;
    char **from_files = get_from_files();
    char **to_files = get_to_files();
    err = parse_node_file(node_file, &graph);
    if (err)
        exit(1);
    err = parse_edge_file(edge_file, &graph);
    if (err)
        exit(1);

    int **to_list = get_distance_list(input, to_files, NUMBER_OF_LANDMARKS);
    int **from_list = get_distance_list(input, from_files, NUMBER_OF_LANDMARKS);

    t = clock();
    int nodes_checked = alt(&graph, from_list, to_list, start_node, end_node);
    t = clock() - t;
    double time_taken = ((double)t)/(CLOCKS_PER_SEC/1000);

    printf("Nodes checked: %d \n", nodes_checked);
    printf("Driving time in centi-seconds: %d\n", (graph.n_list + end_node)->d->dist);
    
    print_centi_to_drive_time((graph.n_list + end_node)->d->dist);

    write_path_to_file(&graph, end_node, start_node);

    printf("Time used for ALT: %f ms \n", time_taken);

    for (int i = 0; i < NUMBER_OF_LANDMARKS; i++) {
        free(*(to_list + i));
        free(*(from_list + i));
        free(*(to_files + i));
        free(*(from_files + i));
    }

    free(from_list);
    free(to_list);
    free(from_files);
    free(to_files);
}