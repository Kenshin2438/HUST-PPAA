#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "util.h"

void aligned_block_copy(int64_t * __restrict dst,
                        int64_t * __restrict src,
                        int                  size)
{
#if 1
	printf("Please implemente %s()!\n", __FUNCTION__);
	usleep(500000);
#else
	//TODO
#endif
}

void aligned_block_copy_backwards(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size)
{
#if 1
	printf("Please implemente %s()!\n", __FUNCTION__);
	usleep(500000);
#else
	//TODO
    }
#endif
}

void aligned_block_copy_pf(int64_t * __restrict dst,
                             int64_t * __restrict src,
                             int                  size)
{
#if 1
	printf("Please implemente %s()!\n", __FUNCTION__);
	usleep(500000);
#else 
	//TODO
#endif
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
    int antialias_pattern_mask = (ALIGN_PADDING - 1) & ~(CACHE_LINE_SIZE - 1);
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
#if 1
	printf("Please implemente %s()!\n", __FUNCTION__);
	buf = NULL;
#else
    //TODO
#endif
    
    return buf;
}
