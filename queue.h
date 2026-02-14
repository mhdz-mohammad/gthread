//
// Created by Mohammad Bagher Mhdz on 2/7/2026 A.
//

#ifndef TEST_QUEUE_H
#define TEST_QUEUE_H

#include <stdint.h>

typedef struct gthread_queue_t gt_queue_t;
int gthread_queue_pop(gt_queue_t *q, uintptr_t *val);
int gthread_queue_insert(gt_queue_t *q, uintptr_t val);
int gthread_queue_init(gt_queue_t **q);
int gthread_queue_size(const gt_queue_t *q, uint32_t *size);
void gthread_queue_destroy(gt_queue_t *q);

#endif //TEST_QUEUE_H
