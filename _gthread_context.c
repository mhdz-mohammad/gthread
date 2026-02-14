//
// Created by Mohammad Bagher Mhdz on 1/31/2026 A.
//

#include "_gthread_context.h"
#include "_gthread.h"
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>


typedef struct gthread_uint128_t
{
    uint64_t v[2];
}gt_uint128_t;

typedef struct gthread_arm64_context
{
    //instead of saving all the registers, those that are ,certainly, redundant be discarded
    // what is the difference between callee and preemptive ones

    /*
     * X: general purpose registers
     * V: v0-v31
     * sp: stack pointer
     * pc: program counter
     * pstate: process state, value of conditional labels
     * fpsr: floating point status register
     * fpcr: floating point control register
     */

    gt_uint128_t q8_q15[8];
    uint64_t x19_x28[10];
    uint64_t fp;
    uint64_t lr;
    uint64_t sp;
    uint64_t nzcv;
    uint64_t fpcr;
    uint64_t fpsr;
}gt_arm64_ctx_t;

int gthread_create_ctx(gt_tbc_t *tcb)
{
    gt_arm64_ctx_t *ctx = (gt_arm64_ctx_t*)malloc(sizeof(gt_arm64_ctx_t));
    if (ctx == NULL)
    {
        //TODO: error handling
        errno = ENOMEM;
        return -1;
    }
    uintptr_t sp = (uintptr_t)tcb->stack + tcb->stack_size;
    sp &= ~0xF;
    ctx->sp = sp;

    /*
     * below assignment allows the trampoline to access current thread's data, task and arg,
     * without needing a global variable to watch out for current gthread
     * an step to make the lib reentrant
     */

    ctx->x19_x28[0] = (uint64_t)tcb;
    ctx->lr = (uintptr_t)&gthread_trampoline;
    tcb->ctx = ctx;
    return 0;
}
int gthread_create_raw_ctx(gt_arm64_ctx_t **ctx)
{
    *ctx = (gt_arm64_ctx_t *) malloc(sizeof(gt_arm64_ctx_t));
    if (!(*ctx))
    {
        errno = ENOMEM;
        return -1;
    }
    return 0;
}
