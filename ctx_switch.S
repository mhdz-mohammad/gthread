.text
.global _ctx_switch

_ctx_switch:
    // x0 = old context
    // x1 = new context

    // --------------------
    // Save old context
    // --------------------

    // Save SIMD q8-q15
    stp q8,  q9,  [x0, #0]
    stp q10, q11, [x0, #32]
    stp q12, q13, [x0, #64]
    stp q14, q15, [x0, #96]

    // Save x19-x28
    stp x19, x20, [x0, #128]
    stp x21, x22, [x0, #144]
    stp x23, x24, [x0, #160]
    stp x25, x26, [x0, #176]
    stp x27, x28, [x0, #192]

    // Save FP and LR
    stp x29, x30, [x0, #208]

    // Save SP
    mov x2, sp
    str x2, [x0, #224]

    // Save NZCV
    mrs x2, nzcv
    str x2, [x0, #232]

    // Save FPCR / FPSR
    mrs x2, fpcr
    mrs x3, fpsr
    str x2, [x0, #240]
    str x3, [x0, #248]

    // --------------------
    // Restore new context
    // --------------------

    // Restore SP
    ldr x2, [x1, #224]
    mov sp, x2

    // Restore x19-x28
    ldp x19, x20, [x1, #128]
    ldp x21, x22, [x1, #144]
    ldp x23, x24, [x1, #160]
    ldp x25, x26, [x1, #176]
    ldp x27, x28, [x1, #192]

    // Restore FP and LR
    ldp x29, x30, [x1, #208]

    // Restore SIMD
    ldp q8,  q9,  [x1, #0]
    ldp q10, q11, [x1, #32]
    ldp q12, q13, [x1, #64]
    ldp q14, q15, [x1, #96]

    // Restore FPCR / FPSR
    ldr x2, [x1, #240]
    ldr x3, [x1, #248]
    msr fpcr, x2
    msr fpsr, x3

    // Restore NZCV
    ldr x2, [x1, #232]
    msr nzcv, x2

    ret
