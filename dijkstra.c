#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "io_handling.h"
#include "heap_queue.h"
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

/* ---------- graph functions ---------- */

static void graph_insert_node(struct graph_t *graph, int node_idx, double latitude, double longitude)
{
    struct node_t *node = malloc(sizeof(struct node_t));
    node->node_idx = node_idx;
    node->latitude = latitude;
    node->longitude = longitude;

    graph->node_list[node_idx] = node;
}

static void graph_insert_edge(struct graph_t *graph, int from_idx, int to_idx, int cost)
{
    struct edge_t *edge = malloc(sizeof(struct edge_t));
    edge->from_idx = from_idx;
    edge->to_idx = to_idx;
    edge->cost = cost;
    edge->next = NULL;

    struct edge_t *head = graph->neighbour_list[from_idx];
    if (head == NULL) {
        graph->neighbour_list[from_idx] = edge;
        return;
    }

    /* use new node as linkedlist head */
    edge->next = head;
    graph->neighbour_list[from_idx] = edge;
}

static void graph_print(struct graph_t *graph)
{
    printf("total nodes: %d, total edges: %d.\n", graph->node_count, graph->edge_count);
    printf("[node] list of all edges (to, cost).\n");

    struct edge_t *edge_i;
    for (int i = 0; i < graph->node_count; i++) {
        printf("[%d] ", i);
        edge_i = graph->neighbour_list[i];
        while (edge_i != NULL) {
            printf("(%d, %d) ", edge_i->to_idx, edge_i->cost);
            edge_i = edge_i->next;
        }
        putchar('\n');
    }
    putchar('\n');

}

void graph_free(struct graph_t *graph)
{
    struct edge_t *edge_i, *prev;
    for (int i = 0; i < graph->node_count; i++) {
        edge_i = graph->neighbour_list[i];
        while (edge_i != NULL) {
            prev = edge_i;
            edge_i = edge_i->next;
            free(prev);
        }
    }
    free(graph->neighbour_list);
}


bool parse_node_file(char *file_name, struct graph_t *graph)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        return true;

    int node_count = f_next_int(fp);
    if (node_count == ERROR_EOF || node_count < 1)
        return true;

    graph->node_count = node_count;
    graph->node_list = malloc(node_count * sizeof(struct node_t *));

    while (true) {
        int node_idx = f_next_int(fp);
        double latitude = f_next_double(fp);
        double longitude = f_next_double(fp);
        if (node_idx == ERROR_EOF || latitude == ERROR_EOF || longitude == ERROR_EOF)
            break;

        graph_insert_node(graph, node_idx, latitude, longitude);
    }

    fclose(fp);
    return false;
}

bool parse_edge_file(char *file_name, struct graph_t *graph, bool reversed)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        return true;

    int edge_count = f_next_int(fp);
    if (edge_count == ERROR_EOF || edge_count < 1)
        return true;

    graph->edge_count = edge_count;
    assert(graph->node_count > 0);
    graph->neighbour_list = malloc(graph->node_count * sizeof(struct edge_t *));

    while (true) {
        int from_idx;
        int to_idx;
        if (reversed) {
            to_idx = f_next_int(fp);
            from_idx = f_next_int(fp);
        } else {
            from_idx = f_next_int(fp);
            to_idx = f_next_int(fp);
        }
        int cost = f_next_int(fp);
        f_consume_line(fp);
        if (to_idx == ERROR_EOF || from_idx == ERROR_EOF || cost == ERROR_EOF)
            break;

        graph_insert_edge(graph, from_idx, to_idx, cost);
    }

    fclose(fp);
    return false;
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
    err = parse_edge_file(edge_file, &graph, false);
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

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s <path_to_node_file> <path_to_edge_file> <starting_node>\n",
                argv[0]);
        fprintf(stderr, "example: %s nodes.txt edges.txt 1\n", argv[0]);
        exit(1);
    }

    do_dijkstra(argv[1], argv[2], atoi(argv[3]));
}
