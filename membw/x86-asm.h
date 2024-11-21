#ifndef __X86_ASM_H__
#define __X86_ASM_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void aligned_block_copy_sse2(int64_t * __restrict dst,
                             int64_t * __restrict src,
                             int                  size);
void aligned_block_copy_nt_sse2(int64_t * __restrict dst,
                                int64_t * __restrict src,
                                int                  size);
void aligned_block_copy_nt_pf_sse2(int64_t * __restrict dst,
                                     int64_t * __restrict src,
                                     int                  size);
void aligned_block_fill_nt_sse2(int64_t * __restrict dst,
                                int64_t * __restrict src,
                                int                  size);

#ifdef __cplusplus
}
#endif

#endif
