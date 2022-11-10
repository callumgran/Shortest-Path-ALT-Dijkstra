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
static inline void *heapq_left_child(struct heapq_t *hq, int idx);

/* Method to get the item of the right child in the heap. */
static inline void *heapq_right_child(struct heapq_t *hq, int idx);

/* Method to get the item of the parent in the heap. */
static inline void *heapq_parent(struct heapq_t *hq, int idx);

/* Method to dynamically allocate memory to the heap queue if it is full. */
static void ensure_capacity(struct heapq_t *hq);

/* Method to heapify upwards from a specified node. */
static void heapify_up(struct heapq_t *hq);

/* Method to heapify downwards from a specified node. */
static void heapify_down(struct heapq_t *hq);

/* Method to get the first item in the heap queue without removing the item. */
void *heapq_get(struct heapq_t *hq, int idx);

/* Method to get the first item in the heap queue and remove it. */
void *heapq_pop(struct heapq_t *hq);

/* Method to insert an item into the heap queue. */
void heapq_push(struct heapq_t *hq, void *item);


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
        hq->items = (void **)(realloc(hq->items, hq->capacity * sizeof(void *)));
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
    void *item = heapq_get(hq, 0);
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

void heapq_free(struct heapq_t *hq)
{
    free(hq->items);
    free(hq);
}

struct heapq_t *heapq_malloc(compare_func *cmp)
{
    struct heapq_t *hq = (struct heapq_t *)(malloc(sizeof(struct heapq_t)));
    hq->size = 0;
    hq->capacity = HEAPQ_STARTING_CAPACITY;
    hq->items = (void **)(malloc(HEAPQ_STARTING_CAPACITY * sizeof(void *)));
    hq->cmp = cmp;
    return hq;
}


#endif