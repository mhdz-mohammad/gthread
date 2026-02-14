//
// Created by Mohammad Bagher Mhdz on 1/19/2026 A.
//

#include "priority_queue.h"
#include <stdlib.h>
#include <errno.h>
#include <memory.h>

#define PARENT(X) ((X) >> 1)
#define LEFT_CHILD(X) ((X) << 1)
#define RIGHT_CHILD(X)  ((LEFT_CHILD(X)) + 1)

//TODO: declare and implement improve_key function

typedef struct priority_queue
{
    uintptr_t *arr;
    size_t size;
    size_t capacity;
    pq_attr_t attr;
}pq_t;

void pq_util_swap(uintptr_t  *arr, size_t l, size_t r)
{
    uintptr_t tmp = arr[l];
    arr[l] = arr[r];
    arr[r] = tmp;
}

int pq_create(pq_t **pq, __attribute__((__nonnull__())) pq_attr_t *attr)
{
    *pq = (pq_t *)malloc(sizeof(pq_t));
    if (!*pq)
    {
        errno = ENOMEM;
        return -1;
    }
    pq_t *pq_v = *pq;
    pq_v->size = 0;
    pq_v->capacity = 0;
    pq_v->arr = NULL;
    pq_v->attr = *attr;

    return 0;
}

int pq_util_sift_up(pq_t *pq, uint64_t idx)
{
    if (idx > pq->size)
    {
        errno = EINVAL;
        return -1;
    }
    uint64_t parent = PARENT(idx);
    uintptr_t *arr = pq->arr;
    if (parent == 0) return 0;
    if (pq->attr.compare(arr[parent], arr[idx]) < 0)
    {
        pq_util_swap(arr, parent, idx);
        return pq_util_sift_up(pq, parent);
    }
    return 0;
}
int pq_insert(__attribute__((__nonnull__())) pq_t *pq, uintptr_t new_elem)
{
    /*
     * returns -1, if initially pq was empty but no memory could be allocated to it
     * returns -2 if the queue was not empty but no more memory could be allocated to it
     * return 0 on success
     */
    if (pq->size + 1 >= pq->capacity)
    {
        if (pq->capacity + PQ_INCREASE_AMOUNT_CAP > PQ_MAX_CAP)
        {
            errno = ENOMEM;
            return -1;
        }
        if (pq->capacity == 0)
        {
            pq->capacity = PQ_INIT_CAP + 1; //elem in arr[0] will be unused
            pq->arr = (uintptr_t *) malloc(sizeof(uintptr_t) * (PQ_INIT_CAP + 1));
            if (!pq->arr)
            {
                errno = ENOMEM;
                return -1;
            }
        }else
        {
            uintptr_t *arr = (uintptr_t *) malloc(sizeof(uintptr_t) * (pq->capacity + PQ_INCREASE_AMOUNT_CAP));
            if (!arr)
            {
                errno = ENOMEM;
                return -2;
            }
            memcpy(arr + 1, pq->arr + 1, pq->size * sizeof(uintptr_t));
            free(pq->arr);
            pq->arr = arr;
            pq->capacity += PQ_INCREASE_AMOUNT_CAP;
        }
    }
    pq->size += 1;
    pq->arr[pq->size] = new_elem;
    pq_util_sift_up(pq, pq->size);
    return 0;
}

int pq_heapify(pq_t *pq, uint64_t idx)
{
    size_t size = pq->size;
    if (size < idx)
    {
        errno = EINVAL;
        return -1;
    }
    uint64_t root = idx;
    uint64_t right_child = RIGHT_CHILD(idx);
    uint64_t left_child = LEFT_CHILD(idx);
    uintptr_t *arr = pq->arr;
    if (right_child <= size)
    {
        if(pq->attr.compare(arr[root], arr[right_child]) < 0) root = right_child;
    }
    if (left_child <= size)
    {
        if (pq->attr.compare(arr[root], arr[left_child]) < 0) root = left_child;
    }

    if (root != idx)
    {
        pq_util_swap(pq->arr, root, idx);
        return pq_heapify(pq, root);
    }
    return 0;
}

int pq_pop(pq_t *pq, uintptr_t *top)
{
    /*
     * if the elem exist, at least one element, it will put it in top and return 0
     * otherwise -1 will be returned
     */
    if (pq->size < 1) return -1;
    *top = pq->arr[1];
    pq->arr[1] = pq->arr[pq->size];
    pq->size -= 1;
    pq_heapify(pq, 1);
    return 0;
}

int pq_top(pq_t *pq, uintptr_t *top)
{
    if (pq->size < 1) return -1;
    *top = pq->arr[1];
    return 0;
}

int build_heap(pq_t *pq)
{
    size_t start = pq->size / 2;
    for (size_t i = start; i > 0 ; --i) {
        if(pq_heapify(pq, i) == -1)
            return -1;
    }
    return 0;
}

int pq_traverse_modify(pq_t *pq, int (*modifier)(uintptr_t))
{
    if (pq->size < 1) return -1;
    uintptr_t *arr = pq->arr;
    for (int i = 1; i <= pq->size; ++i) {
        modifier(arr[i]);
    }
    build_heap(pq);
    return 0;
}

int pq_call_elm_destructor(__attribute__((__nonnull__()))pq_t *pq, uintptr_t elm)
{
    if (elm == 0)
    {
        errno = EINVAL;
        return -1;
    }
    pq->attr.destructor(elm);
    return 0;
}
int pq_get_size(__attribute__((__nonnull__()))pq_t *pq, uint32_t *size)
{
    *size = pq->size;
    return 0;
}
