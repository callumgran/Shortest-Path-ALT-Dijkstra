#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alt_preprocess.h"
#include "alt.h"


void print_poi_instructions(char **argv)
{
    printf("To use the command 'poi' use one of these formats.\n");
    printf("%s poi <placename>\n", *(argv));
    printf("%s poi <node_idx> <node_type>\n", *(argv));
    printf("Registered place names and their node types:\n");
    printf("trd_lufthavn\t->\tcharging stations\n");
    printf("trd_torg\t->\tdrinking places\n");
    printf("hemsedal\t->\teating places\n");
    printf("Nodetypes and respective values:\n");
    printf("Placename\t->\t1\n");
    printf("Petrol station\t->\t2\n");
    printf("Charge station\t->\t4\n");
    printf("Eating place\t->\t8\n");
    printf("Drinking place\t->\t16\n");
    printf("Place to sleep\t->\t32\n");
}

void print_preprocess_instructions(char **argv)
{
    printf("Preprocessing must be completed before ALT can be performed.\n");
    printf("To use the command 'preprocess' use this format.\n");
    printf("%s preprocess\n", *(argv));
}

void print_alt_instructions(char **argv)
{
    printf("To use the command 'alt' use this format.\n");
    printf("%s alt <start_node> <end_node>\n", *(argv));
}

void print_dijkstra_instructions(char **argv)
{
    printf("To use the command 'dijkstra' use this format.\n");
    printf("%s dijkstra <start_node> <end_node>\n", *(argv));
}

void print_help_instructions(char **argv)
{
    print_preprocess_instructions(argv);
    putchar('\n');
    print_alt_instructions(argv);
    putchar('\n');
    print_dijkstra_instructions(argv);
    putchar('\n');
    print_poi_instructions(argv);
    putchar('\n');
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf("Try %s help for commands.\n", *(argv));
        return 0;
    }
    if (strcmp(*(argv + 1), "help") == 0) {
        print_help_instructions(argv);
    } else if (strcmp(*(argv + 1), "preprocess") == 0) {
        if (argc == 2)
            preprocess("noder.txt", "kanter.txt");
        else
            print_preprocess_instructions(argv);
    } else if (strcmp(*(argv + 1), "alt") == 0)  {
        if (argc == 4)
            do_alt("noder.txt", "kanter.txt", atoi(*(argv + 2)), atoi(*(argv + 3)));
        else 
            print_alt_instructions(argv);
    } else if (strcmp(*(argv + 1), "dijkstra") == 0) {
        if (argc == 4)
            do_dijkstra("noder.txt", "kanter.txt", atoi(*(argv + 2)), atoi(*(argv + 3)));
        else
            print_dijkstra_instructions(argv);
    } else if (strcmp(*(argv + 1), "poi") == 0) {
        if (argc == 3) {
            if (strcmp(*(argv + 2), "trd_lufthavn") == 0) {
            printf("Finding the 8 closest charging stations to Trondheim lufthavn, Værnes.\n");
            do_dijkstra_poi("noder.txt", "kanter.txt", "interessepkt.txt", 7172108, 4, "trd_lufthavn_poi.csv");
            } else if (strcmp(*(argv + 2), "trd_torg") == 0) {
                printf("Finding the 8 closest drinking places to Trondheim torg.\n");
                do_dijkstra_poi("noder.txt", "kanter.txt", "interessepkt.txt", 4546048, 16, "trd_torg_poi.csv");
            } else if (strcmp(*(argv + 2), "hemsedal") == 0) {
                printf("Finding the 8 closest eating places to Hemsedal.\n");
                do_dijkstra_poi("noder.txt", "kanter.txt", "interessepkt.txt", 3509663, 8, "hemsedal_poi.csv");
            } else {
                print_poi_instructions(argv);
            }
        } else if (argc == 4) {
            printf("Finding the 8 closest places of type: %s to node: %s.\n", *(argv + 3), *(argv + 2));
            do_dijkstra_poi("noder.txt", "kanter.txt", "interessepkt.txt", atoi(*(argv + 2)), atoi(*(argv + 3)), "positions_of_interest.csv");
        } else {
            print_poi_instructions(argv);
        }
    } else {
        printf("Try %s help for commands.\n", *(argv));
    }

    return 0;
}
