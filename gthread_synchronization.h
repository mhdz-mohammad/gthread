//
// Created by Mohammad Bagher Mhdz on 2/7/2026 A.
//

#ifndef TEST_GTHREAD_SYNCHRONIZATION_H
#define TEST_GTHREAD_SYNCHRONIZATION_H

#include <stdint.h>

//TODO: adding attribute to semaphore so that it will support additional datastructures
typedef struct gthread_semaphore gt_semaphore_t;
int gthread_semaphore_wait(gt_semaphore_t *semaphore);
int gthread_semaphore_signal(gt_semaphore_t *semaphore);
#endif //TEST_GTHREAD_SYNCHRONIZATION_H
