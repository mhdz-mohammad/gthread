//
// Created by Mohammad Bagher Mhdz on 1/31/2026 A.
//

#ifndef BANKS__GTHREAD_CONTEXT_H
#define BANKS__GTHREAD_CONTEXT_H

typedef struct gt_tcb gt_tbc_t;
typedef struct gthread_arm64_context gt_arm64_ctx_t;

int gthread_create_raw_ctx(gt_arm64_ctx_t **ctx);
int gthread_create_ctx(gt_tbc_t *tcb);
void ctx_switch(gt_arm64_ctx_t *old_ctx, gt_arm64_ctx_t *new_ctx);

#endif //BANKS__GTHREAD_CONTEXT_H
