#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "heap_queue.h"
#include "graph.h"
#include "dijkstra.h"

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

/* ---------- dijsktra implementation ---------- */
struct shortest_path *dijkstra(struct graph_t *graph, int start_node)
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
    heapq_push_node(hq, start_node, 0);

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

void shortest_path_print(struct shortest_path *sp, int node_count)
{
    printf("[node] [previous node] [cost]\n");
    printf("-----------------------------------\n");
    for (int i = 0; i < node_count; i++) {
        printf("%d\t", i);
        if (sp[i].previous_idx == NODE_START) {
            printf("start\n");
            continue;
        } else if (sp[i].previous_idx == NODE_UNVISITED) {
            printf("unreachable\n");
            continue;
        } else {
            printf("%d\t", sp[i].previous_idx);
        }

        if (sp[i].total_cost == INF)
            printf("ERROR: visited node has total_cost set to infinity\n");
        else
            printf("%d\n", sp[i].total_cost);
    }
}

void do_dijkstra(char *node_file, char *edge_file, int starting_node)
{
    struct graph_t graph;
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
    struct shortest_path *sp = dijkstra(&graph, starting_node);
    shortest_path_print(sp, graph.node_count);
    graph_free(&graph);
    free(sp);
}
