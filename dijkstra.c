/*
 * MIT License: See LICENSE.
 * Copyright (c) 2022 Nicolai Hollup Brand, Callum Gran, Tomáś Beránek
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* compares priority of items in heapqueue. E.I: is a > b ? */
typedef bool (compare_func)(void *a, void *b);

/* min-heap queue inspired by: https://docs.python.org/3/library/heapq.html */
struct heapq_t {
    void **items; 
    int size;
    int capacity;
    compare_func *cmp;
};

struct node_t {
    int node_idx;
    long latitude;
    long longitude;
};

struct edge_t {
    /* index/value/identifer of the node */
    int from_idx;       // superflous in this implementation
    int to_idx;
    int cost;           // time
    struct edge_t *next;
};

struct graph_t {
    struct node_t **node_list;
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
    struct edge_t **neighbour_list;
    int node_count;
    int edge_count;
};


/* ad-hoc datastructure to store information given by performing dijkstra */
struct shortest_path {
    int previous_idx;
    int total_cost;
};

#define HEAPQ_STARTING_CAPACITY 32
#define ERROR_EOF -1
#define INF (1 << 30)

#define NODE_UNVISITED -1
#define NODE_START -2

#define heapq_left_child_idx(parent_idx) (parent_idx * 2 + 1)
#define heapq_right_child_idx(parent_idx) (parent_idx * 2 + 2)
#define heapq_parent_idx(child_idx) ((child_idx - 1) / 2)

#define heapq_has_left(idx, size) (heapq_left_child_idx(idx) < size)
#define heapq_has_right(idx, size) (heapq_right_child_idx(idx) < size)


/* ---------- heap functions ---------- */
static inline void *heapq_left_child(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_left_child_idx(idx)];
}

static inline void *heapq_right_child(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_right_child_idx(idx)];
}

static inline void *heapq_parent(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_parent_idx(idx)];
}

static void swap(struct heapq_t *hq, int a, int b)
{
    void *tmp = hq->items[a];
    hq->items[a] = hq->items[b];
    hq->items[b] = tmp;
}

static void ensure_capacity(struct heapq_t *hq)
{
    if (hq->size == hq->capacity) {
        hq->capacity *= 2;
        hq->items = realloc(hq->items, hq->capacity * sizeof(void *));
    }
}

static void heapify_up(struct heapq_t *hq)
{
    int idx = hq->size - 1;
    int parent_idx = heapq_parent_idx(idx);
    /* keep "repearing" heap as long as parent is greater than child */
    //while (parent_idx >= 0 && hq->items[parent_idx]->cost > hq->items[idx]->cost) {
    while (parent_idx >= 0 && hq->cmp(hq->items[parent_idx], hq->items[idx])) {
        swap(hq, parent_idx, idx);
        /* walk upwards */
        idx = heapq_parent_idx(idx);
        parent_idx = heapq_parent_idx(idx);
    }
}

static void heapify_down(struct heapq_t *hq)
{
    int idx = 0;
    int min_idx;
    while (heapq_has_left(idx, hq->size)) {
        min_idx = heapq_left_child_idx(idx);
        if (heapq_has_right(idx, hq->size) && hq->cmp(hq->items[min_idx], 
                                                      heapq_right_child(hq, idx)))
            min_idx = heapq_right_child_idx(idx);

        //if (hq->items[idx]->cost < hq->items[min_idx]->cost) {
        if (hq->cmp(hq->items[min_idx], hq->items[idx])) {
            break;
        } else {
            swap(hq, idx, min_idx);
            idx = min_idx;
        }
    }
}

void *heapq_get(struct heapq_t *hq, int idx)
{
    if (idx < 0 || idx >= hq->size)
        return NULL;

    return hq->items[idx];
}

void *heapq_pop(struct heapq_t *hq)
{
    struct node_t *item = heapq_get(hq, 0);
    if (item == NULL)
        return NULL;

    hq->items[0] = hq->items[--hq->size];
    heapify_down(hq);
    return item;
}

void heapq_push(struct heapq_t *hq, void *item)
{
    ensure_capacity(hq);
    hq->items[hq->size++] = item;
    heapify_up(hq);
}

//void heapq_push_node(struct heapq_t *hq, int node, int cost_from_start_node)
//{
//    struct node_info *ni = malloc(sizeof(struct node_info));
//    ni->node_idx = node;
//    ni->cost_from_start_node = cost_from_start_node;
//    heapq_push(hq, ni);
//}

void heapq_free(struct heapq_t *hq)
{
    free(hq->items);
    free(hq);
}

struct heapq_t *heapq_malloc(compare_func *cmp)
{
    struct heapq_t *hq = malloc(sizeof(struct heapq_t));
    hq->size = 0;
    hq->capacity = HEAPQ_STARTING_CAPACITY;
    hq->items = malloc(HEAPQ_STARTING_CAPACITY * sizeof(void *));
    hq->cmp = cmp;
    return hq;
}

//bool compare(void *a, void *b)
//{
//    return ((struct node_info *)a)->cost_from_start_node >
//           ((struct node_info *)b)->cost_from_start_node;
//}

/* ---------- graph functions ---------- */
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
//            printf("(%d, %d) ", edge_i->to_idx, edge_i->cost);
//            edge_i = edge_i->next;
//        }
//        putchar('\n');
//    }
//    putchar('\n');
//
//}

static int f_next_int(FILE *fp)
{
    char ch;
    char buf[128] = {0};
    int i = 0;

    /* remove all leading whitespace */
    while ((ch = fgetc(fp)) == ' ') {
    }

    if (ch == EOF)
        return ERROR_EOF;

    buf[i++] = ch;

    do {
        ch = fgetc(fp);
        if (ch == ' ') {
            buf[i] = 0;
            break;
        } else if (ch == '\n') {
            break;
        } else if (i >= 128) {
            fprintf(stderr, "Error parsing file: node longer than 128 chars\n");
            exit(1);
            break;
        }

        buf[i++] = ch;
    } while (ch != EOF);

    if (ch == EOF)
        return ERROR_EOF;

    return atoi(buf);
}

static int f_next_double(FILE *fp)
{
    char ch;
    char buf[128] = {0};
    int i = 0;

    /* remove all leading whitespace */
    while ((ch = fgetc(fp)) == ' ') {
    }

    if (ch == EOF)
        return ERROR_EOF;

    buf[i++] = ch;

    do {
        ch = fgetc(fp);
        if (ch == ' ') {
            buf[i] = 0;
            break;
        } else if (ch == '\n') {
            break;
        } else if (i >= 128) {
            fprintf(stderr, "Error parsing file: node longer than 128 chars\n");
            exit(1);
            break;
        }

        buf[i++] = ch;
    } while (ch != EOF);

    if (ch == EOF)
        return ERROR_EOF;

    return strtod(buf, NULL);
}

void f_consume_line(FILE *fp)
{
    char ch;
    /* consume entire line */
    while ((ch = fgetc(fp))) {
        if (ch == '\n')
            break;
        if (ch == EOF)
            break;
    }
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

struct graph_t *parse_file_into_graph(char *file_name)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        return NULL;

    char ch;
    struct graph_t *graph = malloc(sizeof(struct graph_t));

    int node_count = f_next_int(fp);
    int edge_count = f_next_int(fp);

    /* every node gets its own linked list of edges */
    graph->neighbour_list = malloc(sizeof(struct edge_t *) * node_count);
    /*
     * pernicious bug!: forgetting to set freshly allocated pointers to NULL results in undefined
     * behaviour and uninitialized data will creep in.
     */
    for (int i = 0; i < node_count; i++)
        graph->neighbour_list[i] = NULL;

    while (1) {
        int from_idx = f_next_int(fp);
        int to_idx = f_next_int(fp);
        int cost = f_next_int(fp);
        if (from_idx == ERROR_EOF || to_idx == ERROR_EOF || cost == ERROR_EOF)
            break;
        graph_insert_edge(graph, from_idx, to_idx, cost);
    }

    fclose(fp);
    graph->node_count = node_count;
    graph->edge_count = edge_count;
    return graph;
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
    free(graph);
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

/* ---------- dijsktra implementation ---------- */
/* dijkstra's shortest path from a to all other nodes in the graph */
//struct shortest_path *dijkstra(struct graph_t *graph, int start_node)
//{
//    bool visited[graph->node_count];
//    memset(visited, false, sizeof(bool) * graph->node_count);
//    struct heapq_t *hq = heapq_malloc(compare);
//    struct shortest_path *sp = malloc(sizeof(struct shortest_path) * graph->node_count);
//    /* set initial cost to be as large as possible */
//    for (int i = 0; i < graph->node_count; i++) {
//        sp[i].total_cost = INF;
//        sp[i].previous_idx = NODE_UNVISITED;
//    }
//
//    sp[start_node].total_cost = 0;
//    sp[start_node].previous_idx = NODE_START;
//    heapq_push_node(hq, start_node, 0);
//
//    struct edge_t *neighbour;
//    struct node_info *ni;
//    while ((ni = (struct node_info *)heapq_pop(hq)) != NULL) {
//        visited[ni->node_idx] = true;
//        neighbour = graph->neighbour_list[ni->node_idx];
//
//        /* check all connected neighbours of the current node */
//        while (neighbour != NULL) {
//            /* if a node is already visited, we can't find a shorter path */
//            if (visited[neighbour->to_idx]) {
//                neighbour = neighbour->next;
//                continue;
//            }
//            int new_cost = sp[ni->node_idx].total_cost + neighbour->cost;
//            /* update shortest path is newly calculated cost is less than previously calcualted */
//            if (new_cost < sp[neighbour->to_idx].total_cost) {
//                sp[neighbour->to_idx].previous_idx = ni->node_idx;
//                sp[neighbour->to_idx].total_cost = new_cost;
//                heapq_push_node(hq, neighbour->to_idx, new_cost);
//            }
//            neighbour = neighbour->next;
//        }
//        /* 
//         * why free? heapq_pop() returns a malloced node that would otherwise be lost if it was not
//         * freed here 
//         */
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
        int to_idx = f_next_int(fp);
        int from_idx = f_next_int(fp);
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
    /*
    struct shortest_path *sp;
    struct graph_t *graph = parse_file_into_graph(file_name);
    if (graph == NULL) {
        fprintf(stderr, "error: could not read file '%s'\n", file_name);
        exit(1);
    }

#ifdef DEBUG
    printf("Parsed file %s into graph:\n", file_name);
    graph_print(graph);
#endif

    printf("Dijkstra's algorithm on file '%s' using %d as starting node.\n\n", file_name, 
           starting_node);
    sp = dijkstra(graph, starting_node);
    shortest_path_print(sp, graph->node_count);
    graph_free(graph);
    free(sp);
        */
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s <path_to_node_file> <path_to_edge_file> <starting_node>\n", argv[0]);
        fprintf(stderr, "example: %s nodes.txt edges.txt 1\n", argv[0]);
        exit(1);
    }

    do_dijkstra(argv[1], argv[2], atoi(argv[3]));
}
