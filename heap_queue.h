#ifndef HEAP_QUEUE_H
#define HEAP_QUEUE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* Defines */
#define HEAPQ_STARTING_CAPACITY 32

/* Macros to find the index of children or parent. */
#define heapq_left_child_idx(parent_idx) ((parent_idx << 1) + 1)
#define heapq_right_child_idx(parent_idx) ((parent_idx + 1) << 1)
#define heapq_parent_idx(child_idx) ((child_idx - 1) >> 1)

/* Macro's to check if the heap has children. */
#define heapq_has_left(idx, size) (heapq_left_child_idx(idx) < size)
#define heapq_has_right(idx, size) (heapq_right_child_idx(idx) < size)

/* Macro to swap the place of two items. */
#define swap(a, b)                      \
    do {                                \
        void *c = a;                    \
        a = b;                          \
        b = c;                          \
    } while(0);                         \


/* Typedefs and structs for the heap queue. */

/* Defines a function to compare two items. */
typedef bool (compare_func)(void *a, void *b);

/* Struct for a generic heap queue. */
struct heapq_t {
    void            **items; 
    size_t          size;
    size_t          capacity;
    compare_func    *cmp;
};

/* Methods for the heap queue. */

/* Method to allocate memory to the heap. */
struct heapq_t *heapq_malloc(compare_func *cmp);

/* Destructor method to free the heap. */
void heapq_free(struct heapq_t *hq);

/* Method to get the item of the left child in the heap. */
inline void *heapq_left_child(struct heapq_t *hq, int idx);

/* Method to get the item of the right child in the heap. */
inline void *heapq_right_child(struct heapq_t *hq, int idx);

/* Method to get the item of the parent in the heap. */
inline void *heapq_parent(struct heapq_t *hq, int idx);

/* Method to dynamically allocate memory to the heap queue if it is full. */
void ensure_capacity(struct heapq_t *hq);

/* Method to heapify upwards from a specified node. */
void heapify_up(struct heapq_t *hq);

/* Method to heapify downwards from a specified node. */
void heapify_down(struct heapq_t *hq);

/* Method to get the first item in the heap queue without removing the item. */
void *heapq_get(struct heapq_t *hq, int idx);

/* Method to get the first item in the heap queue and remove it. */
void *heapq_pop(struct heapq_t *hq);

/* Method to insert an item into the heap queue. */
void heapq_push(struct heapq_t *hq, void *item);

#endif