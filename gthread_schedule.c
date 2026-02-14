//
// Created by Mohammad Bagher Mhdz on 2/4/2026 A.
//

#include <errno.h>
#include "gthread_schedule.h"
#include "_gthread.h"

/*
 * Implementation of utilities for programmers
 */

__attribute__((pure))int gthread_get_create_time(uintptr_t tcb, uint64_t *c_time)
{
    //TODO: how to generate the error
    if (tcb == 0)
    {
        errno = EINVAL;
        return -1;
    }
    gt_tcb_t *gtTcb = (gt_tcb_t *)tcb;
    *c_time = gtTcb->creat_time;
    return 0;
}
int gthread_set_estimated_time(uintptr_t tcb, uint64_t est)
{
    /*
     * est is calculated in nanoseconds
     */
    if (tcb == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if (est <= 0)
    {
        errno = EINVAL;
        return -1;
    }
    gt_tcb_t *gtTcb = (gt_tcb_t *)tcb;
    gtTcb->estimated_time = est;
    return 0;
}
__attribute__((pure))int gthread_get_estimated_time(uintptr_t tcb, uint64_t *est)
{
    //TODO:how to generate error
    if (tcb == 0)
    {
        errno = EINVAL;
        return -1;
    }
    gt_tcb_t *gtTcb = (gt_tcb_t *)tcb;
    *est = gtTcb->estimated_time;
    return 0;
}
int gthread_set_priority(uintptr_t tcb, gt_tcb_prio_t prio)
{
    if (tcb == 0)
    {
        errno = EINVAL;
        return -1;
    }
    gt_tcb_t *gtTcb = (gt_tcb_t *)tcb;
    gtTcb->priority = prio;
    return 0;
}
__attribute__((pure))int gthread_get_priority(uintptr_t tcb, gt_tcb_prio_t *prio)
{
    if (tcb == 0)
    {
        errno = EINVAL;
        return -1;
    }
    gt_tcb_t *gtTcb = (gt_tcb_t *)tcb;
    *prio = gtTcb->priority;
    return 0;
}