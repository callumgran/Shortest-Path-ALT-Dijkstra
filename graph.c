#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "graph.h"
#include "io_handling.h"

/* ---------- graph functions ---------- */

void graph_insert_node(struct graph_t *graph, int node_idx, double latitude, double longitude)
{
    struct node_t *node = malloc(sizeof(struct node_t));
    node->node_idx = node_idx;
    node->latitude = latitude;
    node->longitude = longitude;

    graph->node_list[node_idx] = node;
}

void graph_insert_edge(struct graph_t *graph, int from_idx, int to_idx, int cost)
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

void graph_print(struct graph_t *graph)
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

struct graph_t *graph_copy(struct graph_t *graph)
{
    struct graph_t *c_graph = (struct graph_t *)(malloc(sizeof(struct graph_t)));
    c_graph->neighbour_list = (struct edge_t **)(malloc(graph->edge_count * sizeof(struct edge_t)));
    c_graph->node_list = (struct node_t **)(malloc(graph->node_count * sizeof(struct node_t)));
    c_graph->node_count = graph->node_count;
    c_graph->edge_count = graph->edge_count;

    for (int i = 0; i < c_graph->node_count; i++) {
        graph_insert_node(c_graph, graph->node_list[i]->node_idx, 
                            graph->node_list[i]->latitude, graph->node_list[i]->longitude);
        struct edge_t *curr = graph->neighbour_list[i];
        while (curr) {
            graph_insert_edge(c_graph, curr->from_idx, curr->to_idx, curr->cost);
            curr = curr->next;
        }
    }
    return c_graph;
}

struct graph_t *graph_transpose(struct graph_t *graph) 
{
    struct graph_t *t_graph = (struct graph_t *)(malloc(sizeof(struct graph_t)));
    t_graph->neighbour_list = (struct edge_t **)(malloc(graph->edge_count * sizeof(struct edge_t)));
    t_graph->node_list = (struct node_t **)(malloc(graph->node_count * sizeof(struct node_t)));
    t_graph->node_count = graph->node_count;
    t_graph->edge_count = graph->edge_count;

    for (int i = 0; i < t_graph->node_count; i++) {
        graph_insert_node(t_graph, graph->node_list[i]->node_idx, 
                            graph->node_list[i]->latitude, graph->node_list[i]->longitude);
        struct edge_t *curr = graph->neighbour_list[i];
        while (curr) {
            graph_insert_edge(t_graph, curr->to_idx, curr->from_idx, curr->cost);
            curr = curr->next;
        }
    }
    return t_graph;
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

bool parse_edge_file(char *file_name, struct graph_t *graph)
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
        int from_idx = f_next_int(fp);
        int to_idx = f_next_int(fp);
        int cost = f_next_int(fp);
        f_consume_line(fp);
        if (to_idx == ERROR_EOF || from_idx == ERROR_EOF || cost == ERROR_EOF)
            break;

        graph_insert_edge(graph, from_idx, to_idx, cost);
    }

    fclose(fp);
    return false;
}