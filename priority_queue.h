//
// Created by Mohammad Bagher Mhdz on 1/19/2026 A.
//

#ifndef BANKS_PRIORITY_QUEUE_H
#define BANKS_PRIORITY_QUEUE_H

#define PQ_MAX_CAP 1024
#define PQ_INIT_CAP 10
#define PQ_INCREASE_AMOUNT_CAP 20
#include <stdint.h>


typedef struct priority_queue pq_t;



typedef struct pq_attr
{
    int (*compare)(uintptr_t first_candidate, uintptr_t second_candidate);
    int (*destructor)(uintptr_t elem);
}pq_attr_t;


int pq_create(pq_t **pq, pq_attr_t *attr);
int pq_insert(pq_t *pq, uintptr_t new_elem);
int pq_pop(pq_t *pq, uintptr_t *top);
int pq_call_elm_destructor(pq_t *pq, uintptr_t elm);
int pq_top(pq_t *pq, uintptr_t *top);
int pq_traverse_modify(pq_t *pq, int (*modifier)(uintptr_t));
int pq_get_size(pq_t *pq, uint32_t *);



#endif //BANKS_PRIORITY_QUEUE_H
