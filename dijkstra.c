#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "heap_queue.h"
#include "iohandling.h"

typedef bool (compare_func)(void *a, void *b);

struct heapq_t {
    void            **items; 
    size_t          size;
    size_t          capacity;
    compare_func    *cmp;
};

struct edge_t {
    int             from_idx;
    int             to_idx;
    int             m_dist;
    int             t_dist;
    int             spd_lim;
    struct edge_t   *next;
};

struct graph_t {
    struct edge_t   **neighbour_list;
    size_t          node_count;
    size_t          edge_count;
};

struct position_t {
    double          longitude;
    double          latitude;
};

struct node_info_t {
    int             node_idx;
    int             t_tot;
    int             m_tot;
    struct position_t *position;
};

struct node_list_t {
    size_t          len;
    struct node_info_t* nodes;
};

struct shortest_path_t {
    int             prev_idx;
    int             prev_tot;
    int             m_tot;
    int             t_tot;
};

#define INF (1 << 30)
#define ERROR_EOF -1
#define NODE_UNVISITED -1
#define NODE_START -2

bool compare(void *a, void *b)
{
    return ((struct node_info_t *)a)->m_tot >
           ((struct node_info_t *)b)->m_tot;
}

static inline struct node_t *heapq_get_node(struct heapq_t *hq, int idx)
{
    return (struct node_t *)heapq_get(hq, idx);
}

static inline struct node_t *heapq_pop_node(struct heapq_t *hq)
{
    return (struct node_t *)heapq_pop(hq);
}

void heapq_push_node(struct heapq_t *hq, int node, int m_tot)
{
    struct node_info_t *ni = (struct node_info_t *)
                                (malloc(sizeof(struct node_info_t)));
    ni->node_idx = node;
    ni->m_tot = m_tot;
    heapq_push(hq, ni);
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
            printf("(%d, %d, %d) ", edge_i->to_idx, edge_i->m_dist, edge_i->t_dist);
            edge_i = edge_i->next;
        }
        putchar('\n');
    }
    putchar('\n');

}

static void node_list_insert(struct node_list_t *nl, int n_idx, 
                                double latitude, double longitude)
{
    struct node_info_t *n_info;
    struct position_t *p;

    n_info = (struct node_info_t *)(malloc(sizeof(struct node_info_t)));
    p = (struct position_t *)(malloc(sizeof(struct position_t)));

    p->latitude = latitude;
    p->longitude = longitude;

    n_info->node_idx = n_idx;
    n_info->m_tot = 0;
    n_info->t_tot = 0;
    n_info->position = p;

    nl->nodes[n_idx] = *n_info;

    free(n_info);
}

struct node_list_t *parse_nodes(char *file_name)
{
    FILE                *fp;
    size_t              node_count;
    size_t              i;
    int                 n_idx;
    double              latitude;
    double              longitude;
    char                ch;
    struct node_list_t  *nl;

    fp = fopen(file_name, "r");
    if (fp == NULL)
        return NULL;

    node_count = f_next_int(fp);

    nl = (struct node_list_t *)(malloc(sizeof(struct node_list_t)));

    nl->len = node_count;

    nl->nodes = (struct node_info_t *)
                    (calloc(nl->len, sizeof(struct node_info_t)));

    do {
        n_idx = f_next_int(fp);
        latitude = f_next_double(fp);
        longitude = f_next_double(fp);
            
        if (n_idx == ERROR_EOF || latitude == (double)ERROR_EOF || longitude == (double)ERROR_EOF)
            break;

        node_list_insert(nl, n_idx, latitude, longitude);
    } while (1);

    fclose(fp);

    return nl;
}

static void graph_insert(struct graph_t *graph, int from_idx, int to_idx, 
                                int m_dist, int t_dist, int spd_lim)
{
    struct edge_t *edge = malloc(sizeof(struct edge_t));
    edge->from_idx = from_idx;
    edge->to_idx = to_idx;
    edge->m_dist = m_dist;
    edge->t_dist = t_dist;
    edge->spd_lim = spd_lim;
    edge->next = NULL;

    struct edge_t *head = graph->neighbour_list[from_idx];
    if (head == NULL) {
        graph->neighbour_list[from_idx] = edge;
        return;
    }

    edge->next = head;
    graph->neighbour_list[from_idx] = edge;
}

struct graph_t *parse_edges_into_graph(char *file_name, size_t node_count)
{
    FILE        *fp;
    int         from_idx;
    int         to_idx;
    int         t_dist;
    int         m_dist;
    int         spd_lim;
    int         edge_count;
    char        ch;
    struct  graph_t *graph;

    fp = fopen(file_name, "r");
    if (fp == NULL)
        return NULL;

    graph = malloc(sizeof(struct graph_t));

    edge_count = f_next_int(fp);

    graph->neighbour_list = (struct edge_t *)(calloc(node_count, sizeof(struct edge_t *)));


    do {
        from_idx = f_next_int(fp);
        to_idx = f_next_int(fp);
        t_dist = f_next_int(fp);
        m_dist = f_next_int(fp);
        spd_lim = f_next_int(fp);

        if (from_idx == ERROR_EOF || to_idx == ERROR_EOF 
            || t_dist == ERROR_EOF || m_dist == ERROR_EOF 
            || spd_lim == ERROR_EOF)
            break;
        
        graph_insert(graph, from_idx, to_idx, t_dist, m_dist, spd_lim);
    } while (1);

    fclose(fp);

    graph->node_count = node_count;
    graph->edge_count = edge_count;
    return graph;
}


void graph_free(struct graph_t *graph)
{
    struct edge_t   *edge_i, 
                    *prev;
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

struct shortest_path_t *dijkstra(struct graph_t *graph, int start_node)
{
    bool visited[graph->node_count];
    
    memset(visited, false, sizeof(bool) * graph->node_count);
    struct heapq_t *hq = heapq_malloc(compare);
    struct shortest_path_t *sp = malloc(sizeof(struct shortest_path_t) * graph->node_count);

    for (int i = 0; i < graph->node_count; i++) {
        sp[i].m_tot = INF;
        sp[i].prev_idx = NODE_UNVISITED;
    }

    sp[start_node].m_tot = 0;
    sp[start_node].prev_idx = NODE_START;
    heapq_push_node(hq, start_node, 0);

    struct edge_t *neighbour;
    struct node_info_t *ni;
    while ((ni = heapq_pop_node(hq)) != NULL) {
        visited[ni->node_idx] = true;
        neighbour = graph->neighbour_list[ni->node_idx];

        while (neighbour != NULL) {
            if (visited[neighbour->to_idx]) {
                neighbour = neighbour->next;
                continue;
            }
            int new_cost = sp[ni->node_idx].m_tot + neighbour->m_dist;
            if (new_cost < sp[neighbour->to_idx].m_tot) {
                sp[neighbour->to_idx].prev_idx = ni->node_idx;
                sp[neighbour->to_idx].m_tot = new_cost;
                heapq_push_node(hq, neighbour->to_idx, new_cost);
            }
            neighbour = neighbour->next;
        }

        free(ni);
    }

    heapq_free(hq);
    return sp;
}

void do_dijkstra(char *file_name, int starting_node)
{
    struct node_list_t *nl = parse_nodes_info();
    struct shortest_path_t *sp;
    struct graph_t *graph = parse_file_into_graph(file_name, nl->len);
    if (graph == NULL) {
        fprintf(stderr, "error: could not read file '%s'\n", file_name);
        exit(1);
    }

    printf("Dijkstra's algorithm on file '%s' using %d as starting node.\n\n", file_name, 
           starting_node);
    sp = dijkstra(graph, starting_node);
    shortest_path_t_print(sp, graph->node_count);
    graph_free(graph);
    free(sp);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <path_to_input_file> <starting_node>\n", argv[0]);
        fprintf(stderr, "example: %s vg1 1\n", argv[0]);
        exit(1);
    }

    do_dijkstra(argv[1], atoi(argv[2]));
}