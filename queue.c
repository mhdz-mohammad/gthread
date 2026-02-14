//
// Created by Mohammad Bagher Mhdz on 2/7/2026 A.
//

#include "queue.h"
#include <errno.h>
#include <stdlib.h>

#define GT_QUEUE_INIT_SIZE 10


typedef struct gthread_queue_t
{
    uintptr_t *arr;
    uint32_t size;
    uint32_t capacity;
    uint32_t head;
    uint32_t tail;
} gt_queue_t;

/* ---------- INTERNAL ---------- */

static int queue_resize(gt_queue_t *q, uint32_t new_cap)
{
    uintptr_t *new_arr = calloc(new_cap, sizeof(uintptr_t));
    if (!new_arr)
    {
        errno = ENOMEM;
        return -1;
    }

    /* preserve FIFO order */
    for (uint32_t i = 0; i < q->size; i++)
    {
        uint32_t idx = (q->head + i) % q->capacity;
        new_arr[i] = q->arr[idx];
    }

    free(q->arr);

    q->arr = new_arr;
    q->capacity = new_cap;
    q->head = 0;
    q->tail = q->size;

    return 0;
}

/* ---------- PUBLIC ---------- */

int gthread_queue_init(gt_queue_t **q)
{
    *q= (gt_queue_t *) malloc(sizeof(gt_queue_t));
    if (!(*q))
    {
        errno = ENOMEM;
        return -1;
    }
    (*q)->arr = NULL;
    (*q)->size = 0;
    (*q)->capacity = 0;
    (*q)->head = 0;
    (*q)->tail = 0;

    return 0;
}

int gthread_queue_size(const gt_queue_t *q, uint32_t *size)
{
    if (!q)
    {
        return -1;
    }
    *size = q->size;
    return 0;
}

int gthread_queue_insert(gt_queue_t *q, uintptr_t val)
{
    if (!q || !val)
    {
        errno = EINVAL;
        return -1;
    }

    /* lazy allocation */
    if (q->capacity == 0)
    {
        q->arr = calloc(GT_QUEUE_INIT_SIZE, sizeof(uintptr_t));
        if (!q->arr)
        {
            errno = ENOMEM;
            return -1;
        }

        q->capacity = GT_QUEUE_INIT_SIZE;
        q->head = 0;
        q->tail = 0;
    }

    /* resize if full */
    if (q->size == q->capacity)
    {
        if (queue_resize(q, q->capacity * 2) < 0)
            return -1;
    }

    q->arr[q->tail] = val;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;

    return 0;
}

int gthread_queue_pop(gt_queue_t *q, uintptr_t *val)
{
    if (!q || !val)
    {
        errno = EINVAL;
        return -1;
    }

    if (q->size == 0)
    {
        *val = 0;
        return 0; /* empty but not error */
    }

    *val = q->arr[q->head];
    q->arr[q->head] = 0;

    q->head = (q->head + 1) % q->capacity;
    q->size--;

    return 0;
}

void gthread_queue_destroy(gt_queue_t *q)
{
    if (!q)
        return;

    free(q->arr);
    free(q);
}
