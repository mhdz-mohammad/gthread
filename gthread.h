//
// Created by Mohammad Bagher Mhdz on 2/2/2026 A.
//

#ifndef TEST_GTHREAD_H
#define TEST_GTHREAD_H

#include <stdint.h>

typedef struct gthread_pool gthread_pool_t;
/*
 * to make the library safer and preserve protection,
 * gt_scheduler_t has a specific and rigid definition which
 * MUST be obied, However one can define almost all scheduling policies
 * using the structure. here is an indepth explanation:
 *      .data_structure: should be a pointer which will be cast to uintptr
 *      uintptr is chosen to overcome complexity which is involved with using void pointers
 *      it is also faster. One can even devise a multi-level feedback queue with it!
 *
 *      .pick_next: gets the pointer to data_structure and initializes u_tcb with the next gthread_tcb_t
 *      which is cascaded to uintptr to adhere protection. should return -1 if nothing is available.
 *
 *      .insert_task: obvious with previous explanation
 *
 *      !above function should return 0 on success and -1 otherwise, for more robust code fill
 *      errno from errno.h with correct value
 *
 *      .get_size: must return number of elements in the data_structure, will be used in gthread_add_task
 */
typedef struct gt_user_scheduler_st {
    int (* pick_next)(uintptr_t ready_data_structure, uintptr_t blocked_queue, uintptr_t *u_tcb);
    int (* insert_task_ready)(uintptr_t data_structure, uintptr_t);
    int (* insert_task_blocked)(uintptr_t data_structure, uintptr_t);
    int (* on_exit_sched)(uintptr_t data_structure, uintptr_t);
    int (* on_block_sched)(uintptr_t blocked_threads, uintptr_t u_tcb);
    int (* get_size_ready)(uintptr_t data_structure, uint32_t *);
    int (* get_size_blocked)(uintptr_t data_structure, uint32_t *);
    int (* remove_key_blocked)(uintptr_t blocked_threads, uintptr_t u_tcb);
    uintptr_t ready_data_structure;
    uintptr_t blocked_queue;
}gt_usr_sched_t;
typedef struct gt_scheduler_t gt_sched_t;

int gthread_add_task(gthread_pool_t *pool, void (*task)(void *), void *arg);
int gthread_cancel(gthread_pool_t *pool);
int gthread_create(gthread_pool_t **pool, uint32_t workers_num, gt_usr_sched_t *);
void gthread_yield();


int gthread_destroy_thread(uintptr_t tcb);
int gthread_block_sync_stdout(char *string, uint32_t len);
int gthread_join(gthread_pool_t *pool);
int gthread_soft_terminate_join(gthread_pool_t *pool);
int gthread_force_terminate_join(gthread_pool_t *pool);

#endif //TEST_GTHREAD_H
