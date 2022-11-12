#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "graph.h"
#include "astar.h"
#include "heap_queue.h"

static void heapq_push_node(struct heapq_t *hq, int node, int cost_from_start_node)
{
    struct node_info_t *ni = malloc(sizeof(struct node_info_t));
    ni->node_idx = node;
    ni->cost_from_start_node = cost_from_start_node;
    heapq_push(hq, ni);
}

static bool compare(void *a, void *b)
{
    return ((struct edge_t *)a)->cost > ((struct edge_t *)b)->cost;
}

/* Method to get the length of a file and it's contents. */
static struct file_data_t *get_file_data(FILE *input)
{
    struct file_data_t *res = (struct file_data_t *)
                                (malloc(sizeof(struct file_data_t)));

    fseek(input, 0, SEEK_END);
    res->data_len = ftell(input);
    rewind(input);
    res->data = (char *)(malloc(res->data_len));
    fread(res->data, res->data_len, 1, input);

    return res;
}

/* Method to create a 2d array from lists of files containing landmark distances. */
static int **get_distance_list(char **file_names, int landmarks)
{
    FILE *input;
    int **arr;

    arr = (int **)(malloc(landmarks * sizeof(int *)));

    union byte_int_conv byte_int_conv;

    for (int i = 0; i < landmarks; i++) {
        size_t data_len;
        char *data;

        input = fopen(file_names[i], "rb");

        if (input == NULL) {
            fprintf(stderr, "could not open file :(");
            free(arr);
            exit(1);
        }

        struct file_data_t *fd = get_file_data(input);

        fclose(input);
        
        /* The length of the data will always be 1/4 the amount of elements,
        but as the elements are integeres, 4 bytes, we save our selves from division. */
        arr[i] = (int *)(malloc(data_len));

        int k = 0; int l;
        for (int j = 0; j < data_len; j++) {
            if (l == 3) {
                arr[i][k++] = byte_int_conv.c;
                l = 0;
            }
            byte_int_conv.bytes[l++] = data[j];
        }
    }

    return arr;
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
    for (int i = 0; i < 3; i++) {
        int tmp_est = est_dist_l_e(from_list, i, node_idx, node_idx);
        if (tmp_est > est_dist)
            est_dist = tmp_est;
        tmp_est = est_dist_e_l(to_list, i, node_idx, node_idx);
        if (tmp_est > est_dist)
            est_dist = tmp_est;
    }
    return est_dist;
}

/* A-star implementation */
/* Todo: FIX the fucking method */
struct shortest_path *astar(int **from_list, int **to_list, struct graph_t *graph, int start_node, int end_idx)
{
    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);
    struct heapq_t *hq = heapq_malloc(compare);
    struct shortest_path *sp = malloc(sizeof(struct shortest_path) * graph->node_count);
    /* set initial cost to be as large as possible */
    for (int i = 0; i < graph->node_count; i++) {
        sp[i].total_cost = INF;
        sp[i].previous_idx = NODE_UNVISITED;
    }
    
    sp[start_node].total_cost = 0;
    sp[start_node].previous_idx = NODE_START;

    heapq_push_node(hq, start_node, est_dist(from_list, to_list, end_idx, start_node));

    struct edge_t *neighbour;
    struct node_info_t *ni;
    while ((ni = (struct node_info_t *)heapq_pop(hq)) != NULL) {
        visited[ni->node_idx] = true;
        neighbour = graph->neighbour_list[ni->node_idx];

        /* check all connected neighbours of the current node */
        while (neighbour != NULL) {
            /* if a node is already visited, we can't find a shorter path */
            if (visited[neighbour->to_idx]) {
                neighbour = neighbour->next;
                continue;
            }
            int new_cost = sp[ni->node_idx].total_cost + neighbour->cost;
            /* update shortest path is newly calculated cost is less than previously calcualted */
            if (new_cost < sp[neighbour->to_idx].total_cost) {
                sp[neighbour->to_idx].previous_idx = ni->node_idx;
                sp[neighbour->to_idx].total_cost = new_cost;
                heapq_push_node(hq, neighbour->to_idx, new_cost);
            }
            neighbour = neighbour->next;
        }
        /* 
         * why free? heapq_pop() returns a malloced node that would otherwise be lost if it was not
         * freed here 
         */
        free(ni);
    }

    heapq_free(hq);
    return sp;
}
