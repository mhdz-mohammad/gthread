//
// Created by Mohammad Bagher Mhdz on 2/9/2026 A.
//

#include "gthread_linkedlist.h"
#include "gthread_linkedlist_internal.h"
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>


void gthread_list_acquire(__attribute__((nonnull()))gt_list_t *list)
{
    int expected = 1;
    while(
            !atomic_compare_exchange_weak_explicit(&list->accessible,
                                                   &expected, 0,
                                                   memory_order_acquire,
                                                   memory_order_relaxed)
    )
        expected = 1;
}
void gthread_list_release(__attribute__((nonnull()))gt_list_t *list)
{
    atomic_store(&list->accessible, 1);
}


int gt_list_init(gt_list_t **list)
{
    *list = (gt_list_t *) malloc(sizeof(gt_list_t));
    if (!(*list))
    {
        errno = ENOMEM;
        return -1;
    }
    (*list)->head = NULL;
    (*list)->tail = NULL;
    (*list)->size = 0;
    atomic_store(&(*list)->accessible, 1);
    return 0;
}

static gt_list_node_t *gt_list_create_node(uintptr_t key)
{
    gt_list_node_t *node = malloc(sizeof(gt_list_node_t));
    if (!node)
    {
        errno = ENOMEM;
        return NULL;
    }

    node->key = key;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

int gt_list_insert_top(gt_list_t *list, uintptr_t key)
{
    if (!list || key == 0)
    {
        errno = EINVAL;
        return -1;
    }



    gt_list_node_t *node = gt_list_create_node(key);
    if (!node) return -1;

    gthread_list_acquire(list);

    node->next = list->head;

    if (list->head)
        list->head->prev = node;
    else
        list->tail = node;

    list->head = node;
    list->size++;

    gthread_list_release(list);

    return 0;
}

int gt_list_insert_tail(gt_list_t *list, uintptr_t key)
{
    if (!list || key == 0)
    {
        errno = EINVAL;
        return -1;
    }



    gt_list_node_t *node = gt_list_create_node(key);
    if (!node) return -1;

    gthread_list_acquire(list);

    node->prev = list->tail;

    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;

    list->tail = node;
    list->size++;

    gthread_list_release(list);

    return 0;
}

int gt_list_pop_top(gt_list_t *list, uintptr_t *key_out)
{
    if (!list || !key_out || !list->head)
    {
        errno = EINVAL;
        return -1;
    }

    gthread_list_acquire(list);

    gt_list_node_t *node = list->head;

    *key_out = node->key;

    list->head = node->next;

    if (list->head)
        list->head->prev = NULL;
    else
        list->tail = NULL;

    free(node);
    list->size--;

    gthread_list_release(list);

    return 0;
}

int gt_list_pop_tail(gt_list_t *list, uintptr_t *key_out)
{
    if (!list || !key_out || !list->tail)
    {
        errno = EINVAL;
        return -1;
    }

    gthread_list_acquire(list);

    gt_list_node_t *node = list->tail;

    *key_out = node->key;

    list->tail = node->prev;

    if (list->tail)
        list->tail->next = NULL;
    else
        list->head = NULL;

    free(node);
    list->size--;

    gthread_list_release(list);

    return 0;
}

gt_list_node_t *gt_list_find(gt_list_t *list, uintptr_t key)
{
    if (!list || key == 0)
        return NULL;

    gt_list_node_t *cur = list->head;

    while (cur)
    {
        if (cur->key == key)
            return cur;

        cur = cur->next;
    }

    return NULL;
}

int gt_list_remove(gt_list_t *list, uintptr_t key)
{
    if (!list || key == 0)
    {
        errno = EINVAL;
        return -1;
    }

    gthread_list_acquire(list);

    gt_list_node_t *node = gt_list_find(list, key);
    if (!node)
    {
        errno = ENOENT;
        gthread_list_release(list);
        return -1;
    }

    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if (node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    free(node);
    list->size--;

    gthread_list_release(list);

    return 0;
}

void gt_list_destroy(gt_list_t *list)
{
    if (!list) return;

    gthread_list_acquire(list);

    gt_list_node_t *cur = list->head;
    while (cur)
    {
        gt_list_node_t *tmp = cur;
        cur = cur->next;
        free(tmp);
    }

    list->head = list->tail = NULL;
    list->size = 0;

    gthread_list_release(list);
}

int gt_list_size(__attribute((nonnull()))gt_list_t *list, uint32_t *size)
{
    gthread_list_acquire(list);

    *size = list->size;

    gthread_list_release(list);
    return 0;
}

//_______________________Internal____________________________

int gthread_list_init_internal(gt_list_t *list){

    if (!list)
    {
        return -1;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    atomic_store(&list->accessible, 1);
    return 0;
}

int gthread_list_remove(uintptr_t u_list, uintptr_t key){

    gt_list_t *list = (gt_list_t *)u_list;
    if (!list || key == 0)
    {
        errno = EINVAL;
        return -1;
    }

    gthread_list_acquire(list);

    gt_list_node_t *node = gt_list_find(list, key);
    if (!node)
    {
        errno = ENOENT;
        gthread_list_release(list);
        return -1;
    }

    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if (node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    free(node);
    list->size--;

    gthread_list_release(list);
    return 0;
}

int gthread_list_insert_tail(uintptr_t u_list, uintptr_t key)
{
    gt_list_t *list = (gt_list_t *)u_list;
    if (!list || key == 0)
    {
        errno = EINVAL;
        return -1;
    }

    gt_list_node_t *node = gt_list_create_node(key);
    if (!node) return -1;

    gthread_list_acquire(list);

    node->prev = list->tail;

    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;

    list->tail = node;
    list->size++;

    gthread_list_release(list);

    return 0;
}

int gthread_list_size(uintptr_t u_list, uint32_t *size)
{
    if (!u_list)
    {
        errno = EINVAL;
        return 1;
    }
    gt_list_t *list = (gt_list_t *)u_list;

    gthread_list_acquire(list);

    *size = list->size;

    gthread_list_release(list);
    return 0;
}