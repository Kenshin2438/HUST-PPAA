#if defined(__i386__) || defined(__amd64__)

.intel_syntax noprefix
.text

#define PREFETCH_DISTANCE 256

.macro asm_function_helper function_name
    .global \function_name
.func \function_name
\function_name:
#ifdef __amd64__
    .set DST,  rdi
    .set SRC,  rsi
    .set SIZE, rdx
#else
    mov  eax,  [esp + 4]
    mov  ecx,  [esp + 8]
    mov  edx,  [esp + 12]
    .set DST,  eax
    .set SRC,  ecx
    .set SIZE, edx
#endif
.endm

.macro asm_function function_name
#ifdef _WIN32
    asm_function_helper _\function_name
#else
    asm_function_helper \function_name
#endif
.endm

/*****************************************************************************/

asm_function aligned_block_copy_sse2
//TODO
    ret
.endfunc

asm_function aligned_block_copy_nt_sse2
//TODO
    ret
.endfunc

asm_function aligned_block_copy_nt_pf_sse2
//TODO
    ret
.endfunc

asm_function aligned_block_fill_nt_sse2
    movdqa      xmm0,       [SRC + 0]
0:
    movntdq     [DST + 0],  xmm0
    movntdq     [DST + 16], xmm0
    movntdq     [DST + 32], xmm0
    movntdq     [DST + 48], xmm0
    add         DST,        64
    sub         SIZE,       64
    jg          0b
    ret
.endfunc

/*****************************************************************************/

#endif
