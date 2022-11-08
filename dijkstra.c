#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

typedef bool (compare_func)(void *a, void *b);

struct position {
    double longitude;
    double latitude;
};

struct heapq_t {
    void            **items; 
    size_t          size;
    size_t          capacity;
    compare_func    *cmp;
};

struct edge_t {
    uint32_t        from_idx;
    uint32_t        to_idx;
    uint32_t        t_dist;
    uint32_t        t_dist;
    struct edge_t   *next;
};

struct graph_t {
    struct edge_t   **neighbour_list;
    size_t          node_count;
    size_t          edge_count;
};

struct node_info_t {
    uint32_t        node_idx;
    uint32_t        cost_from_start_node;
    struct position *position;
};

struct shortest_path_t {
    uint32_t        previous_idx;
    uint32_t        total_cost;
};

#define HEAPQ_STARTING_CAPACITY 32
#define ERROR_EOF -1
#define INF (1 << 30)

#define NODE_UNVISITED -1
#define NODE_START -2

#define heapq_left_child_idx(parent_idx) ((parent_idx << 1) + 1)
#define heapq_right_child_idx(parent_idx) ((parent_idx + 1) << 1)
#define heapq_parent_idx(child_idx) ((child_idx - 1) >> 1)

#define heapq_has_left(idx, size) (heapq_left_child_idx(idx) < size)
#define heapq_has_right(idx, size) (heapq_right_child_idx(idx) < size)

#define swap(a, b)                      \
    do {                                \
        void *c = a;                    \
        a = b;                          \
        b = c;                          \
    } while(0);                         \

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
    while (parent_idx >= 0 && hq->cmp(hq->items[parent_idx], hq->items[idx])) {
        swap(hq->items[parent_idx], hq->items[idx]);
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

        if (hq->cmp(hq->items[min_idx], hq->items[idx])) {
            break;
        } else {
            swap(hq->items[idx], hq->items[min_idx]);
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

void heapq_push_node(struct heapq_t *hq, int node, int cost_from_start_node)
{
    struct node_info_t *ni = malloc(sizeof(struct node_info_t));
    ni->node_idx = node;
    ni->cost_from_start_node = cost_from_start_node;
    heapq_push(hq, ni);
}

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

bool compare(void *a, void *b)
{
    return ((struct node_info_t *)a)->cost_from_start_node >
           ((struct node_info_t *)b)->cost_from_start_node;
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
            printf("(%d, %d) ", edge_i->to_idx, edge_i->dist_m);
            edge_i = edge_i->next;
        }
        putchar('\n');
    }
    putchar('\n');

}

static int f_next_int(FILE *fp)
{
    char ch;
    char buf[128] = {0};
    int i = 0;

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

static void graph_insert(struct graph_t *graph, int from_idx, int to_idx, int dist_m, int dist_t)
{
    struct edge_t *edge = malloc(sizeof(struct edge_t));
    edge->from_idx = from_idx;
    edge->to_idx = to_idx;
    edge->dist_m = dist_m;
    edge->dist_t = dist_t;
    edge->next = NULL;

    struct edge_t *head = graph->neighbour_list[from_idx];
    if (head == NULL) {
        graph->neighbour_list[from_idx] = edge;
        return;
    }

    edge->next = head;
    graph->neighbour_list[from_idx] = edge;
}

struct graph_t *parse_nodes_into_graph(char *file_name)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        return NULL;

    char ch;
    struct graph_t *graph = malloc(sizeof(struct graph_t));

    int node_count = f_next_int(fp);
    int edge_count = f_next_int(fp);

    graph->neighbour_list = malloc(sizeof(struct edge_t *) * node_count);
    
    for (int i = 0; i < node_count; i++)
        graph->neighbour_list[i] = NULL;

    do {
        int from_idx = f_next_int(fp);
        int to_idx = f_next_int(fp);
        int dist_m = f_next_int(fp);
        int dist_t = f_next_int(fp);
        int spd_lim = f_next_int(fp);
        if (from_idx == ERROR_EOF || to_idx == ERROR_EOF || cost == ERROR_EOF)
            break;
        graph_insert(graph, from_idx, to_idx, cost);
    } while (1);

    fclose(fp);
    graph->node_count = node_count;
    graph->edge_count = edge_count;
    return graph;
}

struct graph_t *parse_edges_into_graph(char *file_name)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        return NULL;

    char ch;
    struct graph_t *graph = malloc(sizeof(struct graph_t));

    int node_count = f_next_int(fp);
    int edge_count = f_next_int(fp);

    graph->neighbour_list = malloc(sizeof(struct edge_t *) * node_count);
    
    for (int i = 0; i < node_count; i++)
        graph->neighbour_list[i] = NULL;

    while (1) {
        int from_idx = f_next_int(fp);
        int to_idx = f_next_int(fp);
        int cost = f_next_int(fp);
        if (from_idx == ERROR_EOF || to_idx == ERROR_EOF || cost == ERROR_EOF)
            break;
        graph_insert(graph, from_idx, to_idx, cost);
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

void shortest_path_t_print(struct shortest_path_t *sp, int node_count)
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

struct shortest_path_t *dijkstra(struct graph_t *graph, int start_node)
{
    bool visited[graph->node_count];
    memset(visited, false, sizeof(bool) * graph->node_count);
    struct heapq_t *hq = heapq_malloc(compare);
    struct shortest_path_t *sp = malloc(sizeof(struct shortest_path_t) * graph->node_count);

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

        while (neighbour != NULL) {
            if (visited[neighbour->to_idx]) {
                neighbour = neighbour->next;
                continue;
            }
            int new_cost = sp[ni->node_idx].total_cost + neighbour->dist_m;
            if (new_cost < sp[neighbour->to_idx].total_cost) {
                sp[neighbour->to_idx].previous_idx = ni->node_idx;
                sp[neighbour->to_idx].total_cost = new_cost;
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
    struct shortest_path_t *sp;
    struct graph_t *graph = parse_file_into_graph(file_name);
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