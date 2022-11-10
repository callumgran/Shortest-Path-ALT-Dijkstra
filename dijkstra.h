

/* Defines */
#define INF (1 << 30)
#define NODE_UNVISITED -1
#define NODE_START -2

/* Structs */
struct node_t {
    int node_idx;
    long latitude;
    long longitude;
};

struct node_info_t {
    int node_idx;
    int cost_from_start_node;
};

struct edge_t {
    /* index/value/identifer of the node */
    int from_idx;
    int to_idx;
    int cost;
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

/* Methods */

/* Method to free graph and all nodes and edges. */
void graph_free(struct graph_t *graph);

/* Method to parse nodes into graph from file. */
/* Return true if an error occurs. */
bool parse_node_file(char *file_name, struct graph_t *graph);

/* Method to parse edges into graph from file. */
/* Return true if an error occurs. */
bool parse_edge_file(char *file_name, struct graph_t *graph);

/* Dijkstra method */
/* Finds the shortest path from a to all other nodes in a graph from a start node. */
struct shortest_path *dijkstra(struct graph_t *graph, int start_node);

/* Print method to show shortest path to each node in the graph. */
void shortest_path_print(struct shortest_path *sp, int node_count);

/* Initializing function to do dijkstra's algorithm with data. */
void do_dijkstra(char *node_file, char *edge_file, int starting_node);


