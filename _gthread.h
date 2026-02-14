//
// Created by Mohammad Bagher Mhdz on 1/22/2026 A.
//

#ifndef BANKS__GTHREAD_H
#define BANKS__GTHREAD_H

#define STACK_SIZE 16384

#include <sys/signal.h>
#include <pthread.h>
#include <stdatomic.h>
#include "_gthread_context.h"


typedef enum gt_state
{
    GT_NEW, GT_READY, GT_RUNNING, GT_BLOCKED, GT_TERMINATED
}gt_state_t;
typedef enum sched_policy
{
    SJF, FIFO, RR
}gt_sched_policy_t;

typedef struct gt_scheduler_t gt_sched_t;
typedef struct gt_tcb
{
    u_int32_t id;
    u_int32_t kernel_thread_id; /*will be set whence has been assigned to a kernel level thread, like pthread*/

    /*exe*/
    void (*task)(void *);
    void *arg;

    /*stack*/
    void *stack;
    u_int32_t stack_size;

    /*context*/
    gt_arm64_ctx_t *ctx;

    /*state*/
    gt_state_t state;

    /*sched parameter*/
    u_int64_t creat_time;
    u_int64_t estimated_time;
    u_int64_t priority;
    uintptr_t owner_schd;
    gt_arm64_ctx_t *sched_ctx;
    //TODO: modifying safely so that each knows its parent
    //TODO: making necessary attributes constant

    //synchronization
    /*
     * below two are list of resources which the thread is waiting on
     * or holding them.
     */
    _Atomic uint32_t ref_count;
    uintptr_t waiting_on;
    uintptr_t holding;
}gt_tcb_t;

typedef struct
{
    pthread_mutex_t rsrc_mtx;
    gt_tcb_t *gt_rsrc_owner;

}gt_resource_allocation_log_t;

__attribute__((noreturn)) void gthread_trampoline();
int gthread_tcb_create(gt_tcb_t**, void (*task)(void*), void *);
void gthread_tcb_acquire(gt_tcb_t *t);
void gthread_tcb_release(gt_tcb_t *t);

void gthread_get_current_tcb(gt_tcb_t **tcb);
static inline void gthread_get_tcb(gt_tcb_t **tcb);
static inline u_int64_t gthread_now_ns();





#endif //BANKS__GTHREAD_H
