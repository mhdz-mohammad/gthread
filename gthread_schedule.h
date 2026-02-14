//
// Created by Mohammad Bagher Mhdz on 2/4/2026 A.
//

#ifndef TEST_GTHREAD_SCHEDULE_H
#define TEST_GTHREAD_SCHEDULE_H

#include <stdint.h>

/*
 * below functions/attributes are developed to help user implement more flexible and broader range of
 * scheduling methods.
 * all of them return -1 on failure and 0 otherwise. corresponding error can be retrieved from errno.h
 */
typedef enum gt_tcb_priority
{
    /*
     * interpretation of below priorities is programmer's responsibility, however I highly recommend
     * to use 0 as the lowest priority and 5 as the highest so that comparison will be easier.
     */
    GT_PRIO_0, GT_PRIO_1, GT_PRIO_2, GT_PRIO_3, GT_PRIO_4, GT_PRIO_5
}gt_tcb_prio_t;

/*
 * NOTE:
 * Output parameters must point to valid, writable memory.
 *
 * Correct usage:
 *     gt_prio_t prio;
 *     gthread_get_priority(tcb, &prio);
 *
 * Undefined behavior:
 *     gt_prio_t *prio;
 *     gthread_get_priority(tcb, prio);
 */
int gthread_get_create_time(uintptr_t tcb, uint64_t *c_time);
int gthread_set_estimated_time(uintptr_t tcb, uint64_t est);
int gthread_get_estimated_time(uintptr_t tcb, uint64_t *est);
int gthread_set_priority(uintptr_t tcb, gt_tcb_prio_t prio);
int gthread_get_priority(uintptr_t tcb, gt_tcb_prio_t *prio);

#endif //TEST_GTHREAD_SCHEDULE_H
