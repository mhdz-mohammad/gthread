//
// Created by Mohammad Bagher Mhdz on 2/9/2026 A.
//

#ifndef TEST_GTHREAD_LINKEDLIST_H
#define TEST_GTHREAD_LINKEDLIST_H

#include <stdint.h>

typedef struct gt_list_node gt_list_node_t;
typedef struct gt_list gt_list_t;

//TODO: free list in list_destroy
// 2- adding compare function


int gt_list_init(gt_list_t **list);
int gt_list_insert_top(gt_list_t *list, uintptr_t key);
int gt_list_insert_tail(gt_list_t *list, uintptr_t key);
int gt_list_pop_top(gt_list_t *list, uintptr_t *key_out);
int gt_list_pop_tail(gt_list_t *list, uintptr_t *key_out);
int gt_list_remove(gt_list_t *list, uintptr_t key);
int gt_list_size(gt_list_t *list, uint32_t *size);
void gt_list_destroy(gt_list_t *list);
#endif //TEST_GTHREAD_LINKEDLIST_H
