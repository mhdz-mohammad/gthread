//
// Created by Mohammad Bagher Mhdz on 2/3/2026 A.
//

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "SJF.h"
#include "gthread.h"
#include "gthread_schedule.h"
#include "priority_queue.h"
#include "gthread_linkedlist.h"
#include "gthread_linkedlist_internal.h"

int compare(uintptr_t u_tcb1, uintptr_t u_tcb2)
{
    if (u_tcb1 == 0 || u_tcb2 == 0)
    {
        errno = EINVAL;
        exit(-1);
    }
    u_int64_t c_time1, c_time2;
    gthread_get_create_time(u_tcb1, &c_time1);
    gthread_get_create_time(u_tcb2, &c_time2);
    if (c_time1 > c_time2) return -1;
    return 0;
}

int destructor(uintptr_t u_tcb)
{
    return gthread_destroy_thread(u_tcb);
}
int gthread_sjf_insert_task(uintptr_t u_pq, uintptr_t u_tcb)
{
    if (u_pq == 0 || u_tcb == 0)
    {
        errno = EINVAL;
        return -1;
    }
    pq_t *pq = (pq_t *)u_pq;
    if(pq_insert(pq, u_tcb) < 0)
    {
        return -1;
    }
    char *str = "thread created successfully\n";
    gthread_block_sync_stdout(str, strlen(str));
    return 0;
}

int gthread_sjf_get_size(uintptr_t u_pq, uint32_t *size)
{
    if (u_pq == 0)
    {
        errno = EINVAL;
        return -1;
    }
    pq_t *pq = (pq_t *)u_pq;
    return pq_get_size(pq, size);

}

int gthread_sjf_pick_next(uintptr_t u_pq, uintptr_t u_blocked, uintptr_t *u_tcb)
{
    if (u_pq == 0)
    {
        errno = EINVAL;
        return -1;
    }
    pq_t *pq = (pq_t *)u_pq;
    if(pq_pop(pq, u_tcb) < 0)
    {
        *u_tcb = 0;
        return 0;
    }
    char *str = "thread is running\n";
    gthread_block_sync_stdout(str, strlen(str));
    return 0;
    }

int gthread_sjf_on_exit_sched(uintptr_t u_pq, uintptr_t tcb)
{
    if (u_pq == 0 || tcb == 0)
    {
        errno = EINVAL;
        return -1;
    }
    char *str = "thread terminated\n";
    gthread_block_sync_stdout(str, strlen(str));
    int ret = gthread_destroy_thread(tcb);
    if (ret < 0)
    {
        char *str = "thread couldn't be destroyed\n";
        gthread_block_sync_stdout(str, strlen(str));
    }else
    {
        char *str = "thread destroyed successfully\n";
        gthread_block_sync_stdout(str, strlen(str));
    }

    return ret;
}
int gthread_sjf_create(gt_usr_sched_t  **sched)
{
    *sched = (gt_usr_sched_t *) malloc(sizeof(gt_usr_sched_t));
    if(!(*sched))
    {
        errno = ENOMEM;
        return -1;
    }

    pq_attr_t *pqAttr = (pq_attr_t *) malloc(sizeof(pq_attr_t));
    pq_t *pq; //ready queue
    gt_list_t *list; //blocked list

    pqAttr->compare = compare;
    pqAttr->destructor = destructor;

    pq_create(&pq, pqAttr);
    gt_list_init(&list);

    (*sched)->ready_data_structure = (uintptr_t)pq;
    (*sched)->blocked_queue = (uintptr_t)list;
    (*sched)->get_size_ready = gthread_sjf_get_size;
    (*sched)->get_size_blocked = gthread_list_size;
    (*sched)->pick_next= gthread_sjf_pick_next;
    (*sched)->insert_task_ready = gthread_sjf_insert_task;
    (*sched)->insert_task_blocked = gthread_list_insert_tail;
    (*sched)->on_exit_sched = gthread_sjf_on_exit_sched;
    (*sched)->remove_key_blocked = gthread_list_remove;

    return 0;
}