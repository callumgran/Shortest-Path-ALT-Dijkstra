#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "heap_queue.h"
#include "graph.h"
#include "pathfinding_utils.h"
#include "dijkstra.h"

/* ---------- dijsktra implementations ---------- */

void dijkstra_pre_process(struct graph_t *graph, int start_node)
{
    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);
    struct heapq_t *hq = heapq_malloc(compare);
    init_prev(graph, start_node);

    heapq_push_node(hq, start_node, 0);

    struct node_info_t *ni;
    while ((ni = (struct node_info_t *)heapq_pop(hq))) {

        visited[ni->node_idx] = true;

        for (struct edge_t *edge = graph->n_list[ni->node_idx].first_edge; edge; edge = edge->next_edge) {
            if (!visited[edge->to_node->node_idx]) {
                int new_cost = graph->n_list[ni->node_idx].d->dist + edge->cost;
                if (edge->to_node->d->dist > new_cost) {
                    edge->to_node->d->dist = new_cost;
                    edge->to_node->d->prev = &graph->n_list[ni->node_idx];
                    heapq_push_node(hq, edge->to_node->node_idx, new_cost);
                }
            }
        }
        free(ni);
    }

    heapq_free(hq);
}

static int *dijkstra_poi(struct graph_t *graph, int start_node, int node_type)
{
    int *nodes = (int *)(malloc(8 * sizeof(int)));
    int nodes_size = 0;

    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);
    struct heapq_t *hq = heapq_malloc(compare);
    init_prev(graph, start_node);

    heapq_push_node(hq, start_node, 0);

    struct node_info_t *ni;

    while ((ni = (struct node_info_t *)heapq_pop(hq))) {

        if (!visited[ni->node_idx]) {
            if ((graph->n_list[ni->node_idx].code & node_type) == node_type) {
                nodes[nodes_size++] = ni->node_idx;
                if (nodes_size == 8) {
                    free(ni);
                    break;
                }
            }
        }

        visited[ni->node_idx] = true;
            
        for (struct edge_t *edge = graph->n_list[ni->node_idx].first_edge; edge; edge = edge->next_edge) {
            if (!visited[edge->to_node->node_idx]) {
                int new_cost = graph->n_list[ni->node_idx].d->dist + edge->cost;
                if (edge->to_node->d->dist > new_cost) {
                    edge->to_node->d->dist = new_cost;
                    edge->to_node->d->prev = &graph->n_list[ni->node_idx];
                    heapq_push_node(hq, edge->to_node->node_idx, new_cost);
                }
            }
        }

        free(ni);
    }

    heapq_free(hq);

    return nodes;
}

/* Method to do dijkstra. */
/* Only runs until the end node is found.*/
/* Returns checked nodes. */
static int dijkstra(struct graph_t *graph, int start_node, int end_node)
{
    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);

    struct heapq_t *hq = heapq_malloc(compare);
    init_prev(graph, start_node);

    heapq_push_node(hq, start_node, 0);

    struct node_info_t *ni;
    int nodes_checked = 0;

    while ((ni = (struct node_info_t *)heapq_pop(hq))->node_idx != end_node) {
        nodes_checked++;
        visited[ni->node_idx] = true;

        for (struct edge_t *edge = graph->n_list[ni->node_idx].first_edge; edge; edge = edge->next_edge) {
            if (!visited[edge->to_node->node_idx]) {
                int new_cost = graph->n_list[ni->node_idx].d->dist + edge->cost;
                if (edge->to_node->d->dist > new_cost) {
                    edge->to_node->d->dist = new_cost;
                    edge->to_node->d->prev = &graph->n_list[ni->node_idx];
                    heapq_push_node(hq, edge->to_node->node_idx, (new_cost));
                }
            }
        }

        free(ni);
    }

    heapq_free(hq);

    return nodes_checked;
}

void do_dijkstra_poi(char *node_file, char *edge_file, char* poi_file, 
                        int start_node, int node_type, char* out_file)
{
    struct graph_t graph;
    bool err;
    err = parse_node_file(node_file, &graph);
    if (err)
        exit(1);
    err = parse_edge_file(edge_file, &graph);
    if (err)
        exit(1);
    err = parse_poi_file(poi_file, &graph);
    if (err)
        exit(1);

    int *poi = dijkstra_poi(&graph, start_node, node_type);

    struct node_t *node;
    for (int i = 0; i < 8; i++) {
        node = &graph.n_list[poi[i]];
        printf("POI %d: %s of POI type: %d\n", i + 1, node->name, node_type);
    }

    write_poi_to_file(&graph, poi, 8, out_file);

    free(poi);

    graph_free(&graph);
}

void do_dijkstra(char *node_file, char *edge_file, int start_node, int end_node)
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

    t = clock();
    int nodes_checked = dijkstra(&graph, start_node, end_node);
    t = clock() - t;
    double time_taken = ((double)t)/(CLOCKS_PER_SEC/1000);

    printf("Nodes checked: %d \n", nodes_checked);
    printf("Driving time in centi-seconds: %d\n", graph.n_list[end_node].d->dist);
    
    print_centi_to_drive_time(graph.n_list[end_node].d->dist);

    write_path_to_file(&graph, end_node, start_node);

    printf("Time used for Dijkstra: %f ms \n", time_taken);

    graph_free(&graph);
}