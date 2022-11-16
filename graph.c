#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "graph.h"
#include "io_handling.h"

/* ---------- graph functions ---------- */

void graph_insert_poi(struct graph_t *graph, int node_idx, int node_code, char* name)
{
    graph->n_list[node_idx].code = node_code;
    graph->n_list[node_idx].name = strdup(name);
}

void graph_insert_node(struct graph_t *graph, int node_idx, float latitude, float longitude)
{
    struct node_t *node = malloc(sizeof(struct node_t));
    node->node_idx = node_idx;
    node->latitude = latitude;
    node->longitude = longitude;
    node->name = NULL;
    node->code = 0;

    graph->n_list[node_idx] = *node;
}

void graph_insert_edge(struct graph_t *graph, int from_idx, int to_idx, int cost)
{
    struct edge_t *edge = malloc(sizeof(struct edge_t));
    edge->to_node = &graph->n_list[to_idx];
    edge->next_edge = graph->n_list[from_idx].first_edge;
    edge->cost = cost;
    graph->n_list[from_idx].first_edge = edge;
}

void graph_print(struct graph_t *graph)
{
    printf("total nodes: %d, total edges: %d.\n", graph->node_count, graph->edge_count);
    printf("[node] list of all edges (to, cost).\n");

    for (int i = 0; i < graph->node_count; i++) {
        printf("[%d] ", i);
        struct node_t *curr = &graph->n_list[i];
        for (struct edge_t *edge = curr->first_edge; edge; edge = edge->next_edge)
            printf("(%d, %d) ", edge->to_node->node_idx, edge->cost);

        putchar('\n');
    }
    putchar('\n');

}

struct graph_t *graph_transpose(struct graph_t *graph)
{
    struct graph_t *t_graph = (struct graph_t *)(malloc(sizeof(struct graph_t)));
    t_graph->n_list = (struct node_t *)(malloc(graph->node_count * sizeof(struct node_t)));
    t_graph->node_count = graph->node_count;
    t_graph->edge_count = graph->edge_count;

    for (int i = 0; i < t_graph->node_count; i++)
        graph_insert_node(t_graph, i, graph->n_list[i].latitude, graph->n_list[i].longitude);

    for (int i = 0; i < t_graph->node_count; i++)
        for (struct edge_t *edge = graph->n_list[i].first_edge; edge; edge = edge->next_edge)
            graph_insert_edge(t_graph, edge->to_node->node_idx, i, edge->cost);

    return t_graph;
}

struct graph_t *graph_copy(struct graph_t *graph)
{
    struct graph_t *c_graph = (struct graph_t *)(malloc(sizeof(struct graph_t)));
    c_graph->n_list = (struct node_t *)(malloc(graph->node_count * sizeof(struct node_t)));
    c_graph->node_count = graph->node_count;
    c_graph->edge_count = graph->edge_count;

    for (int i = 0; i < c_graph->node_count; i++)
        graph_insert_node(c_graph, i, graph->n_list[i].latitude, graph->n_list[i].longitude);

    for (int i = 0; i < c_graph->node_count; i++)
        for (struct edge_t *edge = graph->n_list[i].first_edge; edge; edge = edge->next_edge)
            graph_insert_edge(c_graph, i, edge->to_node->node_idx, edge->cost);

    return c_graph;
}

void graph_free(struct graph_t *graph)
{
    struct edge_t *edge_i, *prev;
    for (int i = 0; i < graph->node_count; i++) {
        edge_i = graph->n_list[i].first_edge;
        while (edge_i != NULL) {
            prev = edge_i;
            edge_i = edge_i->next_edge;
            free(prev);
        }
        if (graph->n_list[i].name != NULL)
            free(graph->n_list[i].name);
        free(graph->n_list[i].d);
    }
    free(graph->n_list);
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
    graph->n_list = (struct node_t *)(malloc(node_count * sizeof(struct node_t)));

    while (true) {
        int node_idx = f_next_int(fp);
        float latitude = f_next_float(fp);
        float longitude = f_next_float(fp);
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

bool parse_poi_file(char *file_name, struct graph_t *graph)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        return true;

    int poi_count = f_next_int(fp);
    if (poi_count == ERROR_EOF || poi_count < 1)
        return true;

    while (true) {
        int node_idx = f_next_int(fp);
        int node_code = f_next_int(fp);
        char* name = f_next_str(fp);
        void f_consume_line(FILE *fp);
        if (node_idx == ERROR_EOF || node_code == ERROR_EOF || name == NULL)
            break;

        // printf("Code: %d Name: %s\n", node_code, name);
        graph_insert_poi(graph, node_idx, node_code, name);
    }
    // printf("--------------------------------------\n");
    // printf("DONE\n");
    // printf("--------------------------------------\n");
    fclose(fp);
    return false;
}