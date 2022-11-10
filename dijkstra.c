/*
 * MIT License
 * Copyright (c) 2022 Nicolai Hollup Brand, Callum Gran, Tomáś Beránek.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "heap_queue.h"
#include "iohandling.h"


struct edge_t {
    int from_idx;
    int to_idx;
    int cost;           // time
    struct edge_t *next;
};

struct graph_t {
    struct node_t   **node_list;
    /*
     * every node in the node_list has a linked list of edges.
     * the value at the index of the neighbour_list is the head of the linked list of edges.
     *
     * example:
     * [0]      -> edge -> edge -> edge
     * [1]      -> edge
     * [2]      -> NULL
     * [3]      -> edge -> edge
     * ...
     * [i]
     */
    struct edge_t   **neighbour_list;
    size_t          node_count;
    size_t          edge_count;
};

struct node_t {
    int node_idx;
    long latitude;
    long longitude;
};

struct shortest_path_t {
    int        prev_idx;
    int        prev_tot;
    int        m_tot;
    int        t_tot;
};

#define INF (1 << 30)

#define NODE_UNVISITED -1
#define NODE_START -2


//bool compare(void *a, void *b)
//{
//    return ((struct node_info_t *)a)->m_tot >
//           ((struct node_info_t *)b)->m_tot;
//}

//void graph_print(struct graph_t *graph)
//{
//    printf("total nodes: %d, total edges: %d.\n", graph->node_count, graph->edge_count);
//    printf("[node] list of all edges (to, cost).\n");
//
//    struct edge_t *edge_i;
//    for (int i = 0; i < graph->node_count; i++) {
//        printf("[%d] ", i);
//        edge_i = graph->neighbour_list[i];
//        while (edge_i != NULL) {
//            printf("(%d, %d, %d) ", edge_i->to_idx, edge_i->m_dist, edge_i->t_dist);
//            edge_i = edge_i->next;
//        }
//        putchar('\n');
//    }
//    putchar('\n');
//
//}

void graph_free(struct graph_t *graph)
{
    struct edge_t   *edge_i, *prev;
    int             i;

    for (i = 0; i < graph->node_count; i++) {
        edge_i = graph->neighbour_list[i];
        while (edge_i != NULL) {
            prev = edge_i;
            edge_i = edge_i->next;
            free(prev);
        }
    }

    free(graph->neighbour_list);
    free(graph);
}

void shortest_path_t_print(struct shortest_path_t *sp, int node_count)
{
    int         i;

    printf("[node] [previous node] [cost]\n");
    printf("-----------------------------------\n");

    for (i = 0; i < node_count; i++) {
        printf("%d\t", i);
        if (sp[i].prev_idx == NODE_START) {
            printf("start\n");
            continue;
        } else if (sp[i].prev_idx == NODE_UNVISITED) {
            printf("unreachable\n");
            continue;
        } else {
            printf("%d\t", sp[i].prev_idx);
        }

        if (sp[i].m_tot == INF)
            printf("ERROR: visited node has m_tot set to infinity\n");
        else
            printf("%d\n", sp[i].m_tot);
    }
}

//struct shortest_path_t *dijkstra(struct graph_t *graph, int start_node)
//{
//    bool visited[graph->node_count];
//    
//    memset(visited, false, sizeof(bool) * graph->node_count);
//    struct heapq_t *hq = heapq_malloc(compare);
//    struct shortest_path_t *sp = malloc(sizeof(struct shortest_path_t) * graph->node_count);
//
//    for (int i = 0; i < graph->node_count; i++) {
//        sp[i].m_tot = INF;
//        sp[i].prev_idx = NODE_UNVISITED;
//    }
//
//    sp[start_node].m_tot = 0;
//    sp[start_node].prev_idx = NODE_START;
//    heapq_push_node(hq, start_node, 0);
//
//    struct edge_t *neighbour;
//    struct node_info_t *ni;
//    while ((ni = (struct node_info_t *)heapq_pop(hq)) != NULL) {
//        visited[ni->node_idx] = true;
//        neighbour = graph->neighbour_list[ni->node_idx];
//
//        while (neighbour != NULL) {
//            if (visited[neighbour->to_idx]) {
//                neighbour = neighbour->next;
//                continue;
//            }
//            int new_cost = sp[ni->node_idx].m_tot + neighbour->m_dist;
//            if (new_cost < sp[neighbour->to_idx].m_tot) {
//                sp[neighbour->to_idx].prev_idx = ni->node_idx;
//                sp[neighbour->to_idx].m_tot = new_cost;
//                heapq_push_node(hq, neighbour->to_idx, new_cost);
//            }
//            neighbour = neighbour->next;
//        }
//
//        free(ni);
//    }
//
//    heapq_free(hq);
//    return sp;
//}

void graph_insert_node(struct graph_t *graph, int node_idx, double latitude, double longitude)
{
    struct node_t *node = malloc(sizeof(struct node_t));
    node->node_idx = node_idx;
    node->latitude = latitude;
    node->longitude = longitude;

    graph->node_list[node_idx] = node;
}

/*
 * returns true if error, else false
 */
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

/*
 * returns true if error, else false
 */
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

    //struct node_list_t *nl = parse_nodes_info();
    //struct shortest_path_t *sp;
    //struct graph_t *graph = parse_file_into_graph(file_name, nl->len);
    //if (graph == NULL) {
    //    fprintf(stderr, "error: could not read file '%s'\n", file_name);
    //    exit(1);
    //}

    //printf("Dijkstra's algorithm on file '%s' using %d as starting node.\n\n", file_name, 
    //       starting_node);
    //sp = dijkstra(graph, starting_node);
    //shortest_path_t_print(sp, graph->node_count);
    //graph_free(graph);
    //free(sp);
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s <path_to_node_file> <path_to_edge_file> <starting_node>\n", argv[0]);
        fprintf(stderr, "example: %s noder.txt kanter.txt 1\n", argv[0]);
        exit(1);
    }

    do_dijkstra(argv[1], argv[2], atoi(argv[3]));
}
