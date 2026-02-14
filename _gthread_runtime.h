//
// Created by Mohammad Bagher Mhdz on 2/11/2026 A.
//

#ifndef TEST__GTHREAD_RUNTIME_H
#define TEST__GTHREAD_RUNTIME_H



typedef struct gthread_runtime gt_runtime_t;

void gthread_runtime_destroy(gt_runtime_t *runtime);
int gthread_runtime_foreach_tcb(gt_runtime_t *runtime, void (*task)(void *arg, gt_tcb_t *), void *arg);
int gthread_runtime_init(gt_runtime_t *runtime);
int gthread_runtime_add_tcb(gt_runtime_t *runtime, gt_tcb_t *tcb);
int gthread_runtime_remove_tcb(gt_runtime_t *runtime, gt_tcb_t *tcb);

#endif //TEST__GTHREAD_RUNTIME_H
