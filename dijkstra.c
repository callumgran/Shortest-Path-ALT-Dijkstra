#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "heap_queue.h"
#include "graph.h"
#include "dijkstra.h"

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

/* ---------- dijsktra implementations ---------- */
void dijkstra_pre_process(struct graph_t *graph, int start_node)
{
    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);
    struct heapq_t *hq = heapq_malloc(compare);
    init_prev(graph, start_node);

    heapq_push_node(hq, start_node, 0);

    struct node_t *curr;
    struct node_info_t *ni;
    while ((ni = (struct node_info_t *)heapq_pop(hq))) {

        visited[ni->node_idx] = true;
            
        curr = &graph->n_list[ni->node_idx];
        for (struct edge_t *edge = curr->first_edge; edge; edge = edge->next_edge) {
            if (visited[edge->to_node->node_idx])
                continue;

            int new_cost = curr->d->dist + edge->cost;
            if (edge->to_node->d->dist > new_cost) {
                edge->to_node->d->dist = new_cost;
                edge->to_node->d->prev = curr;
                heapq_push_node(hq, edge->to_node->node_idx, new_cost);
            }
        }
        /* 
         * why free? heapq_pop() returns a malloced node that would otherwise be lost if it was not
         * freed here 
         */
        free(ni);
    }

    heapq_free(hq);
}


int dijkstra(struct graph_t *graph, int start_node, int end_node)
{
    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);
    struct heapq_t *hq = heapq_malloc(compare);
    init_prev(graph, start_node);

    heapq_push_node(hq, start_node, 0);

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
                heapq_push_node(hq, edge->to_node->node_idx, new_cost);
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

void do_dijkstra(char *node_file, char *edge_file, int starting_node, int end_node)
{
    struct graph_t graph;
    clock_t t;
    bool err;
    err = parse_node_file(node_file, &graph);
    if (err)
        exit(1);
    err = parse_edge_file(edge_file, &graph);
    if (err)
        exit(1);


//#ifdef DEBUG
//    printf("Parsed files %s %s into graph:\n", node_file, edge_file);
//    graph_print(&graph);
//#endif

#ifdef VERBOSE
    printf("Dijkstra's algorithm on file node file '%s' and edge file '%s' using %d as starting \
            node.\n\n", node_file, edge_file, starting_node);
#endif
    t = clock();
    int nodes_checked = dijkstra(&graph, starting_node, end_node);
    t = clock() - t;
    double time_taken = ((double)t)/(CLOCKS_PER_SEC/1000);

    printf("Nodes checked: %d \n", nodes_checked);
    printf("Driving time in centi-seconds: %d\n", graph.n_list[end_node].d->dist);
    int hours = (graph.n_list[end_node].d->dist) / 360000;
    int mins = (graph.n_list[end_node].d->dist - (hours * 360000))/6000;
    int secs = (graph.n_list[end_node].d->dist - (hours * 360000) - (mins * 6000))/100;
    
    printf("Driving time: %d:%d:%d\n", hours, mins, secs);

    struct node_t *node = &graph.n_list[end_node];
    
    while (node->node_idx != starting_node) {
        printf("%d <- ", node->node_idx);
        node = node->d->prev;
    }
    
    printf("%d", starting_node);
    printf("\n");

    printf("Time used for Dijkstra: %f ms \n", time_taken);

    graph_free(&graph);
}