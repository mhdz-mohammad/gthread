//
// Created by Mohammad Bagher Mhdz on 2/11/2026 A.
//

#ifndef TEST_GTHREAD_LINKEDLIST_INTERNAL_H
#define TEST_GTHREAD_LINKEDLIST_INTERNAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include "gthread_linkedlist.h"

typedef struct gt_list_node {
    uintptr_t key;
    struct gt_list_node *prev;
    struct gt_list_node *next;
} gt_list_node_t;

typedef struct gt_list {
    gt_list_node_t *head;
    gt_list_node_t *tail;
    uint32_t size;
    _Atomic int accessible;
} gt_list_t;

int gthread_list_init_internal(gt_list_t *list);
int gthread_list_remove(uintptr_t list, uintptr_t key);
int gthread_list_insert_tail(uintptr_t u_list, uintptr_t key);
int gthread_list_size(uintptr_t u_list, uint32_t *size);
#endif //TEST_GTHREAD_LINKEDLIST_INTERNAL_H
