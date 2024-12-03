#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>

#include "util.h"

void aligned_block_copy(int64_t * __restrict dst,
                        int64_t * __restrict src,
                        int                  size)
{
    assert(size % sizeof(int64_t) == 0);
    size /= sizeof(int64_t);
    for (int i = 0; i < size; i++) dst[i] = src[i];
}

void aligned_block_copy_backwards(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size)
{
    assert(size % sizeof(int64_t) == 0);
    size /= sizeof(int64_t);
    for (int i = size - 1; i >= 0; i--) dst[i] = src[i];
}

void aligned_block_copy_pf(int64_t * __restrict dst,
                           int64_t * __restrict src,
                           int                  size)
{
    assert(size % (sizeof(int64_t) * 8) == 0);
    size /= sizeof(int64_t);
    for (int i = 0; i < size; i += 8) {
        // __builtin_prefetch (const void *addr[, rw[, locality]])
        // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
        __builtin_prefetch(src + i + 32, 0, 1);
        __builtin_prefetch(dst + i + 32, 1, 1);
        dst[i + 0] = src[i + 0];
        dst[i + 1] = src[i + 1];
        dst[i + 2] = src[i + 2];
        dst[i + 3] = src[i + 3];
        dst[i + 4] = src[i + 4];
        dst[i + 5] = src[i + 5];
        dst[i + 6] = src[i + 6];
        dst[i + 7] = src[i + 7];
    }
}

void aligned_block_fill(int64_t * __restrict dst,
                        int64_t * __restrict src,
                        int                  size)
{
    int64_t data = *src;
    while ((size -= 64) >= 0)
    {
        *dst++ = data;
        *dst++ = data;
        *dst++ = data;
        *dst++ = data;
        *dst++ = data;
        *dst++ = data;
        *dst++ = data;
        *dst++ = data;
    }
}

double gettime(void)
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (double)((int64_t)tv.tv_sec * 1000000 + tv.tv_usec) / 1000000.;
}


#define ALIGN_PADDING    0x100000
#define CACHE_LINE_SIZE  128

static char *align_up(char *ptr, int align)
{
    return (char *)(((uintptr_t)ptr + align - 1) & ~(uintptr_t)(align - 1));
}

void *alloc_four_aligned_buffers(void **buf1_, int size1,
                                 void **buf2_, int size2,
                                 void **buf3_, int size3,
                                 void **buf4_, int size4)
{
    char **buf1 = (char **)buf1_, **buf2 = (char **)buf2_;
    char **buf3 = (char **)buf3_, **buf4 = (char **)buf4_;

    char *buf, *ptr;

    if (!buf1 || size1 < 0)
        size1 = 0;
    if (!buf2 || size2 < 0)
        size2 = 0;
    if (!buf3 || size3 < 0)
        size3 = 0;
    if (!buf4 || size4 < 0)
        size4 = 0;

    ptr = buf = 
        (char *)malloc(size1 + size2 + size3 + size4 + 9 * ALIGN_PADDING);

    ptr = align_up(ptr, ALIGN_PADDING);
    // reference: https://embeddedartistry.com/blog/2017/02/22/generating-aligned-memory/
#define PRINT(ptr) printf("[addr] " #ptr " = %p\n", ptr)
    if (buf1) {
        *buf1 = ptr;
        PRINT(*buf1);
        ptr = align_up(ptr + size1, ALIGN_PADDING);
    }
    if (buf2) {
        *buf2 = ptr;
        PRINT(*buf2);
        ptr = align_up(ptr + size2, ALIGN_PADDING);
    }
    if (buf3) {
        *buf3 = ptr;
        PRINT(*buf3);
        ptr = align_up(ptr + size3, ALIGN_PADDING);
    }
    if (buf4) {
        *buf4 = ptr;
        PRINT(*buf4);
        ptr = align_up(ptr + size4, ALIGN_PADDING);
    }
#undef PRINT
    return buf;
}
