//
// Created by Mohammad Bagher Mhdz on 2/7/2026 A.
//


#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "gthread_synchronization.h"
#include "gthread_sched_internal.h"
#include "_gthread.h"
#include "queue.h"
#include "gthread_linkedlist.h"


typedef struct gthread_semaphore
{
    pthread_mutex_t mutex;
    int32_t value;
    gt_queue_t *q;
}gt_semaphore_t;

static void gthread_block()
{
    gt_tcb_t *tcb;
    gthread_get_current_tcb(&tcb);

    tcb->state = GT_BLOCKED;
    ctx_switch(tcb->ctx, tcb->sched_ctx);

}
int gthread_semaphore_init(__attribute__((nonnull()))gt_semaphore_t **gtSemaphore, int32_t value)
{
    if (value <=0)
    {
        errno = EINVAL;
        return -1;
    }
    *gtSemaphore = (gt_semaphore_t *) malloc(sizeof(gt_semaphore_t));
    if (!(*gtSemaphore))
    {
        errno = ENOMEM;
        return -1;
    }
    (*gtSemaphore)->value = value;
    if (pthread_mutex_init(&(*gtSemaphore)->mutex, NULL) != 0)
        goto fail;
    if (gthread_queue_init(&(*gtSemaphore)->q) == -1)
        goto fail;
    return 0;

    fail:
        return -1;
}
int gthread_semaphore_wait(__attribute__((__nonnull__()))gt_semaphore_t *semaphore) {
    int e = pthread_mutex_lock(&semaphore->mutex);
    if(e != 0)
    {
        errno = e;
        return -1;
    }
    semaphore->value -= 1;

    gt_tcb_t *tcb;
    gthread_get_current_tcb(&tcb);

    if (semaphore->value >= 0) {
        gt_list_insert_tail((gt_list_t *)tcb->holding, (uintptr_t)semaphore);

        gt_list_remove((gt_list_t *)tcb->waiting_on, (uintptr_t)semaphore);
        pthread_mutex_unlock(&semaphore->mutex);
        return 0;
    }
    gthread_tcb_acquire(tcb); //ref count
    //semaphore acquire it
    if(gt_list_insert_tail((gt_list_t *)tcb->waiting_on, (uintptr_t)semaphore) != 0)
        goto fail;
    if(gthread_queue_insert(semaphore->q, (uintptr_t)tcb) != 0)
        goto fail;

    pthread_mutex_unlock(&semaphore->mutex);

    gthread_block();

    return 0;

    fail:
        pthread_mutex_unlock(&semaphore->mutex);
        return -1;
}

int gthread_semaphore_signal(__attribute__((nonnull()))gt_semaphore_t *semaphore)
{
    int e = pthread_mutex_lock(&semaphore->mutex);
    if (e != 0)
    {
        errno = e;
        return -1;
    }
    semaphore->value += 1;

    //current tcb
    gt_tcb_t *tcb;
    gthread_get_current_tcb(&tcb);

    gt_list_remove((gt_list_t *)tcb->holding, (uintptr_t)semaphore); //removing from holding list

    //choosing candidate for awaking
    uintptr_t u_next_tcb;
    gt_tcb_t *next_tcb;
    for (; ; ) {
        gthread_queue_pop(semaphore->q, &u_next_tcb);
        if (u_next_tcb == 0) {
            pthread_mutex_unlock(&semaphore->mutex);
            return 0;
        }
        next_tcb = (gt_tcb_t*)u_next_tcb;

        if (next_tcb->state == GT_TERMINATED)
        {
            gthread_tcb_release(next_tcb);
            continue;
        }
        break;
    }
    pthread_mutex_unlock(&semaphore->mutex);

    gt_list_remove((gt_list_t *)next_tcb->waiting_on, (uintptr_t)semaphore);
    gt_list_insert_tail((gt_list_t*)next_tcb->holding, (uintptr_t)semaphore);

    //it might have freed resources during solving a deadlock, so the thread is waiting on multiple resources

    uint32_t size;
    if (gt_list_size((gt_list_t*)next_tcb->waiting_on, &size) != 0){
        return -1;
    }
    if (size == 0)
    {
        next_tcb->state = GT_READY;
        gt_sched_t *sched = (gt_sched_t *)next_tcb->owner_schd;

        pthread_mutex_lock(sched->usr_sched_mutex);

        if(sched->usr_sched.insert_task_ready
        (sched->usr_sched.ready_data_structure, (uintptr_t)tcb) != 0
        )
            goto failure;

        if(sched->usr_sched.remove_key_blocked
        (sched->usr_sched.blocked_queue, (uintptr_t)tcb) != 0
        )
            goto failure;

        pthread_mutex_unlock(sched->usr_sched_mutex);

        failure:
            pthread_mutex_unlock(sched->usr_sched_mutex);
            return -1;
    }
    
    return 0;

}


