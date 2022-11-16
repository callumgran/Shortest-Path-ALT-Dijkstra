#ifndef ALT_H
#define ALT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* Methods */

/* Method to run the ALT algorithm with files and nodes. */
void do_alt(char *node_file, char *edge_file, int start_node, int end_node);

#endif