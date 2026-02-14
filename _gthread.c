//
// Created by Mohammad Bagher Mhdz on 1/22/2026 A.
//

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdatomic.h>
#include "_gthread.h"
#include "_gthread_context.h"
__attribute__((noreturn)) void gthread_trampoline()
{
    gt_tcb_t *tcb = NULL;
    gthread_get_tcb(&tcb);
    if (tcb == NULL)
    {
        errno = EACCES;
        exit(-1);
    }

    tcb->task(tcb->arg);
    tcb->state = GT_TERMINATED;

    ctx_switch(tcb->ctx, tcb->sched_ctx); //going back to scheduler
    __builtin_unreachable();
}

int gthread_tcb_create(gt_tcb_t **gtTcb, void (*task)(void*), void *arg)
{
    /*
     * creates a new user thread, green thread, and writes it on the provided gt_tcb_t argument
     * returns -1 on failure, errno.h will be initialized with corresponding error code
     */

    //TODO: adding stack size to parameter arguments

    u_int32_t id = 0; //more robust and multithread safe implementation
                             // using a constant specifying maximum gthread number for each application/thread
    *gtTcb = (gt_tcb_t *) malloc(sizeof(gt_tcb_t));
    gt_tcb_t *gtTcb_ptr = *gtTcb;

    gtTcb_ptr->state = GT_NEW;
    gtTcb_ptr->creat_time = gthread_now_ns();
    gtTcb_ptr->task = task;
    gtTcb_ptr->arg = arg;
    gtTcb_ptr->stack = (void*) malloc(STACK_SIZE);
    gtTcb_ptr->stack_size = STACK_SIZE;

    //synchronization parameters
    atomic_store(&gtTcb_ptr->ref_count, 1);
    gtTcb_ptr->waiting_on = 0;
    gtTcb_ptr->holding = 0;
    gthread_create_ctx(gtTcb_ptr);

    //TODO: checking return value of functions
    return 0;
}

static inline void gthread_get_tcb(gt_tcb_t **tcb)
{
   __asm__ volatile ("mov %0, x19": "=r"(*tcb));
}

void gthread_get_current_tcb(gt_tcb_t **tcb)
{
    gthread_get_tcb(tcb);
}
static inline uint64_t gthread_now_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}


void gthread_tcb_acquire(gt_tcb_t *t)
{
    atomic_fetch_add_explicit(&t->ref_count, 1, memory_order_relaxed);
}

void gthread_tcb_release(gt_tcb_t *t)
{
    if (atomic_fetch_sub_explicit(&t->ref_count, 1, memory_order_acq_rel) == 1)
        free(t);
}
