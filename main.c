#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alt_preprocess.h"
#include "dijkstra.h"


int main(int argc, char **argv)
{
    if (argc == 1) {
    bad_input:
        fprintf(stderr, "bad input :-(\n");
        fprintf(stderr, "usage: %s (preprocess | shortest-path | points-of-interest)\
                         <path-to-node-file> <path-to-edge-file> (start_node) (end_node)", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "preprocess") == 0) {
        if (argc < 4) {
            fprintf(stderr, "bad input :-(\n");
            fprintf(stderr, "usage: %s preprocess <path-to-node-file> <path-to-edge-file>", argv[0]);
            return 1;
        }
        preprocess("island_noder.txt", "island_kanter.txt");
        return 0;
    }

    if (strcmp(argv[1], "shortest-path") == 0) {
        if (argc < 6) {
            fprintf(stderr, "bad input :-(\n");
            fprintf(stderr, "usage: %s shortest-path <path-to-node-file> <path-to-edge-file>\
                             <start_node> <end_node>", argv[0]);
            return 1;
        }
        //do_dijkstra();
        //do_ALT();
        return 0;
    }

    if (strcmp(argv[1], "points-of-interest") == 0) {
        if (argc < 5) {
            fprintf(stderr, "bad input :-(\n");
            fprintf(stderr, "usage: %s points-of-interest <path-to-node-file> <path-to-edge-file>\
                             <start_node>", argv[0]);
            return 1;
        }
        return 0;
    }

    goto bad_input;
}
