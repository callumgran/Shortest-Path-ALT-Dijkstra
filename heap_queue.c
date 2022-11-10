#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "heap_queue.h"

inline void *heapq_left_child(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_left_child_idx(idx)];
}

inline void *heapq_right_child(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_right_child_idx(idx)];
}

inline void *heapq_parent(struct heapq_t *hq, int idx)
{
    return hq->items[heapq_parent_idx(idx)];
}

void ensure_capacity(struct heapq_t *hq)
{
    if (hq->size == hq->capacity) {
        hq->capacity *= 2;
        hq->items = (void **)(realloc(hq->items, hq->capacity * sizeof(void *)));
    }
}

void heapify_up(struct heapq_t *hq)
{
    int idx = hq->size - 1;
    int parent_idx = heapq_parent_idx(idx);
    while (parent_idx >= 0 && hq->cmp(hq->items[parent_idx], hq->items[idx])) {
        swap(hq->items[parent_idx], hq->items[idx]);
        idx = heapq_parent_idx(idx);
        parent_idx = heapq_parent_idx(idx);
    }
}

void heapify_down(struct heapq_t *hq)
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
