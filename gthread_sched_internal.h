//
// Created by Mohammad Bagher Mhdz on 2/12/2026 A.
//

#ifndef TEST_GTHREAD_SCHED_INTERNAL_H
#define TEST_GTHREAD_SCHED_INTERNAL_H

#include <pthread.h>
#include "gthread.h"
#include "_gthread_context.h"

typedef struct gt_scheduler_t{
    pthread_mutex_t *usr_sched_mutex; //to synchronize access to data_structure in usr_sched
    //inserting and picking up wil be synchronized
    gt_usr_sched_t usr_sched;
    gt_arm64_ctx_t *sched_ctx;
}gt_sched_t;

typedef struct gthread_pool
{
    size_t n_workers;
    pthread_t *workers;
    gt_sched_t *schedulers;
}gthread_pool_t;


#endif //TEST_GTHREAD_SCHED_INTERNAL_H
