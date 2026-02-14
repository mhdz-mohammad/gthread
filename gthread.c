//
// Created by Mohammad Bagher Mhdz on 2/2/2026 A.
//

#include "_gthread_context.h"
#include "_gthread.h"
#include "gthread.h"
#include "gthread_sched_internal.h"
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <string.h>
char *init= "initializing scheduler loop\n";
char *main_loop = "entering main loop\n";
char *picking_element = "picking element\n";
char *picked = "element picked \n";
char *ret = "returned";
//TODO: adding macros to choose correct context for the base architecture, final phase
//TODO: implementing error handling for
// .malloc




void* scheduler_loop(__attribute__((__nonnull__()))void *v_sched)
{
    //TODO: adding a boolean or a flag which indicates the scheduler is out of work and will be slept to be
    // woken up after insertion of another task. Should be postponed until phase two,
    // after devising blocking queue and mutexes

    //TODO: considering a flag which tells the scheduler whether to terminate immediately or after its queue got empty,
    // during waiting for the data_structure to became empty, the scheduler accepts no tasks.

    gt_sched_t *sched = (gt_sched_t *)v_sched;

    /*
     * short-keys for user defined functions
     * this result in less indirect access during the execution and better demand paging handling
     */
    int (* pickNext)(uintptr_t, uintptr_t, uintptr_t *) = sched->usr_sched.pick_next;
    int (* insert_ready)(uintptr_t, uintptr_t) = sched->usr_sched.insert_task_ready;
    int (* insert_blocked)(uintptr_t, uintptr_t) = sched->usr_sched.insert_task_blocked;
    int (* on_exit_sched)(uintptr_t, uintptr_t) = sched->usr_sched.on_exit_sched;
    int (* on_block_sched)(uintptr_t, uintptr_t) = sched->usr_sched.on_block_sched;
    int (* getSizeReady)(uintptr_t, uint32_t *) = sched->usr_sched.get_size_ready;
    int (* getSizeBlocked)(uintptr_t, uint32_t *) = sched->usr_sched.get_size_blocked;
    int (* removeKeyBlocked)(uintptr_t, uintptr_t) = sched->usr_sched.remove_key_blocked;

    uintptr_t ready_data_structure = sched->usr_sched.ready_data_structure;
    uintptr_t blocked_queue = sched->usr_sched.blocked_queue;
    /*
     * short-keys for sched_ctx
     * less indirect access
     */
    gt_arm64_ctx_t *sched_ctx = sched->sched_ctx;
    pthread_mutex_t * u_mutex = sched->usr_sched_mutex;
    /*
     * variables to use during the scheduling loop
     */
    uintptr_t u_next;
    gt_tcb_t *tcb_next;
    int func_ret;
    for ( ; ; ) {

        pthread_mutex_lock(u_mutex);
        func_ret = pickNext(ready_data_structure, blocked_queue, &u_next);
        pthread_mutex_unlock(u_mutex);

        if (func_ret < 0)
        {
            //TODO: more robust error handling, final phase
            exit(-2);
        }
        if (!u_next) {
            continue;
        }

        tcb_next = (gt_tcb_t *)(u_next);
        tcb_next->state = GT_RUNNING;
        ctx_switch(sched_ctx, tcb_next->ctx);


        if (tcb_next->state == GT_TERMINATED)
        {
            gthread_tcb_release(tcb_next);

            pthread_mutex_lock(u_mutex);
            on_exit_sched(ready_data_structure, u_next);
            pthread_mutex_unlock(u_mutex);
        }
        else if (tcb_next->state == GT_BLOCKED)
        {

            pthread_mutex_lock(u_mutex);
            on_block_sched(blocked_queue, u_next);
            pthread_mutex_unlock(u_mutex);
        }



    }
}
int gthread_create( gthread_pool_t **pool_d,
                    uint32_t workers_num,
                    __attribute__((__nonnull__())) struct gt_user_scheduler_st *usr_scheds)
{
    //TODO: more detailed error handling
    if (workers_num<= 0)
    {
        errno = EINVAL;
        return -1;
    }
    *pool_d = (gthread_pool_t*) malloc(sizeof(gthread_pool_t));

    gthread_pool_t *pool = *pool_d;
    pthread_t *workers =  (pthread_t*) calloc(workers_num, sizeof(pthread_t));
    gt_sched_t *scheds = (gt_sched_t *) calloc(workers_num, sizeof(gt_sched_t));


    for (int i = 0; i < workers_num; ++i) {
        scheds[i].usr_sched = usr_scheds[i];
        scheds[i].usr_sched_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(scheds[i].usr_sched_mutex, NULL);
        if(gthread_create_raw_ctx(&scheds[i].sched_ctx) < 0)
        {
            exit(-1);
        }
    }

    for (int i = 0; i < workers_num; ++i) {
        pthread_create(&workers[i], NULL, scheduler_loop, (void *)&scheds[i]);
    }

    pool->n_workers = workers_num;
    pool->workers = workers;
    pool->schedulers = scheds;
    return 0;
}

int add_task(__attribute__((__nonnull__()))gthread_pool_t *pool, void (*task)(void *), void *arg)
{
    //TODO: more robust error handling, final phase
    //TODO: different methods of adding tasks, not necessary choosing the most free one
    gt_tcb_t *tcb;
    if(gthread_tcb_create(&tcb, task, arg) < 0)
    {
        return -1;
    }

    size_t idx, min = SIZE_MAX;
    uint32_t size;
    gt_sched_t *sched = pool->schedulers;
    for (size_t i = 0; i < pool->n_workers; ++i) {
        gt_usr_sched_t *us_sched = &sched[i].usr_sched;

        pthread_mutex_lock(sched[i].usr_sched_mutex);
        us_sched->get_size_ready(us_sched->ready_data_structure, &size);
        pthread_mutex_unlock(sched[i].usr_sched_mutex);

        if (size <= min)
        {
            min = size;
            idx = i;
        }
    }
    tcb->owner_schd = (uintptr_t)&sched[idx];
    tcb->sched_ctx = sched[idx].sched_ctx;
    pthread_mutex_lock(sched[idx].usr_sched_mutex);
    sched[idx].usr_sched.insert_task_ready(
            sched[idx].usr_sched.ready_data_structure,
            (uintptr_t)tcb);
    pthread_mutex_unlock(sched[idx].usr_sched_mutex);
    return 0;
}


int gthread_block_sync_stdout(__attribute__((__nonnull__()))char *string, uint32_t len)
{
    if (len <= 0)
    {
        errno = EINVAL;
        return -1;
    }
    static pthread_mutex_t mutex;
    static int init = 0;
    if (init == 0)
    {
        pthread_mutex_init(&mutex, NULL);
        init = 1;
    }

    pthread_mutex_lock(&mutex);
    write(STDOUT_FILENO, string, len);
    pthread_mutex_unlock(&mutex);

    return 0;
}

int gthread_destroy_thread(uintptr_t u_tcb)
{
    if (u_tcb == 0)
    {
        errno = EINVAL;
        return -1;
    }
    gt_tcb_t *gtTcb = (gt_tcb_t *)u_tcb;
    gthread_tcb_release(gtTcb);
    return 0;
}
int gthread_add_task(__attribute__((__nonnull__()))gthread_pool_t *pool, void (*task)(void *), void *arg)
{
    gt_tcb_t  *tcb;
    gthread_tcb_create(&tcb, task, arg);
    tcb->sched_ctx = pool->schedulers[0].sched_ctx;
    tcb->owner_schd = (uintptr_t)&pool->schedulers[0];
    pthread_mutex_lock(pool->schedulers[0].usr_sched_mutex);
    pool->schedulers[0].usr_sched.insert_task_ready(pool->schedulers[0].usr_sched.ready_data_structure, (uintptr_t)tcb);
    pthread_mutex_unlock(pool->schedulers[0].usr_sched_mutex);
    return 0;
}

int gthread_join(__attribute__((__nonnull__()))gthread_pool_t *pool)
{
    uint32_t size = pool->n_workers;
    pthread_t *arr = pool->workers;

    for (int i = 0; i < size; ++i) {
        pthread_join(arr[i], NULL);
    }
    return 0;
}

void gthread_yield()
{
    gt_tcb_t *tcb;
    gthread_get_current_tcb(&tcb);
    tcb->state = GT_READY;

    ctx_switch(tcb->ctx, tcb->sched_ctx);
}




