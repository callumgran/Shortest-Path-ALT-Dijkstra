#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alt_preprocess.h"


int main(int argc, char **argv)
{
    if (strcmp(argv[1], "preprocess") == 0)
        // preprocess("island_noder.txt", "island_kanter.txt");
        preprocess("norden_noder.txt", "norden_kanter.txt");

    //if (argc != 4) {
    //    fprintf(stderr, "usage: %s <path_to_node_file> <path_to_edge_file> <starting_node>\n",
    //            argv[0]);
    //    fprintf(stderr, "example: %s nodes.txt edges.txt 1\n", argv[0]);
    //    exit(1);
    //}

    //do_dijkstra(argv[1], argv[2], atoi(argv[3]));
}
