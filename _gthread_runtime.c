//
// Created by Mohammad Bagher Mhdz on 2/11/2026 A.
//

#include "_gthread.h"
#include "_gthread_runtime.h"
#include "gthread_linkedlist_internal.h"
#include <pthread.h>
#include <errno.h>


typedef struct gthread_runtime
{
    pthread_mutex_t mutex;
    gt_list_t list;

}gt_runtime_t;

int gthread_runtime_foreach_tcb(gt_runtime_t *runtime, void (* task)(void *arg, gt_tcb_t *tcb), void *arg)
{
    gt_list_node_t *cur = runtime->list.head;
    uintptr_t key;
    while(cur)
    {
        key = cur->key;
        if (!key)
        {
            errno = EINVAL;
            return -1;
        }
        task(arg, (gt_tcb_t *)key);

        cur = cur->next;
    }
    return 0;
}

int gthread_runtime_init(gt_runtime_t *runtime)
{
    if(pthread_mutex_init(&runtime->mutex, NULL) != 0)
        goto failure;
    if(gthread_list_init_internal(&runtime->list) != 0)
        goto failure;
    return 0;

    failure:
        return -1;
}

void gthread_runtime_destroy(gt_runtime_t *runtime)
{
    pthread_mutex_lock(&runtime->mutex);
    gt_list_destroy(&runtime->list);
    pthread_mutex_unlock(&runtime->mutex);
    pthread_mutex_destroy(&runtime->mutex);
}

int gthread_runtime_add_tcb(__attribute__((nonnull()))gt_runtime_t *runtime,
                            __attribute__((nonnull()))gt_tcb_t *tcb)
{
    uintptr_t u_tcb = (uintptr_t)tcb;
    pthread_mutex_lock(&runtime->mutex);
    if (gt_list_insert_tail(&runtime->list, u_tcb) != 0)
    {
        pthread_mutex_unlock(&runtime->mutex);
        return -1;
    }
    pthread_mutex_unlock(&runtime->mutex);
    return 0;
}

int gthread_runtime_remove_tcb(__attribute__((nonnull()))gt_runtime_t *runtime,
                            __attribute__((nonnull()))gt_tcb_t *tcb)
{
    uintptr_t u_tcb = (uintptr_t)tcb;
    pthread_mutex_lock(&runtime->mutex);
    if (gt_list_remove(&runtime->list, u_tcb) != 0)
    {
        pthread_mutex_unlock(&runtime->mutex);
        return -1;
    }
    pthread_mutex_unlock(&runtime->mutex);
    return 0;
}