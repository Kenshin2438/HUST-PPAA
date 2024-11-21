#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#endif

#include "x86-asm.h"
#include "util.h"

#define SIZE             (32 * 1024 * 1024)
#define BLOCKSIZE        2048
#define COUNT            16
#define MAXREPEATS       10


static void memcpy_wrapper(int64_t *dst, int64_t *src, int size);

typedef struct
{
    const char *description;
    int use_tmpbuf;
    void (*f)(int64_t *, int64_t *, int);
} bench_info;

static bench_info all_bench[] =
{
//================== C implementation ==================
    { "C copy", 0, aligned_block_copy },
    { "C copy backwards", 0, aligned_block_copy_backwards },
    { "C copy prefetched", 0, aligned_block_copy_pf },
    { "C fill", 0, aligned_block_fill },
    { "C memcpy()", 0, memcpy_wrapper },

//================== ASM implementation ==================
    { "SSE2 copy", 0, aligned_block_copy_sse2 },
    { "SSE2 nontemporal copy", 0, aligned_block_copy_nt_sse2 },
    { "SSE2 nontemporal copy prefetched", 0, aligned_block_copy_nt_pf_sse2 },
    { "SSE2 nontemporal fill", 0, aligned_block_fill_nt_sse2 },
    { NULL, 0, NULL }
};



static double bandwidth_bench_helper(int64_t *dstbuf, int64_t *srcbuf,
                                     int64_t *tmpbuf,
                                     int size, int blocksize,
                                     const char *indent_prefix,
                                     int use_tmpbuf,
                                     void (*f)(int64_t *, int64_t *, int),
                                     const char *description)
{
    int i, j, loopcount, n;
    double t1, t2;
    double speed, maxspeed;
    double s, s0, s1, s2;

    /* do up to MAXREPEATS measurements */
    s0 = s1 = s2 = 0;
    maxspeed   = 0;
    for (n = 0; n < MAXREPEATS; n++)
    {
        f(dstbuf, srcbuf, size);
        loopcount = 0;
        t1 = gettime();
        do
        {
            loopcount++;
            if (use_tmpbuf)
            {
                for (i = 0; i < COUNT; i++)
                {
                    for (j = 0; j < size; j += blocksize)
                        {
                        f(tmpbuf, srcbuf + j / sizeof(int64_t), blocksize);
                        f(dstbuf + j / sizeof(int64_t), tmpbuf, blocksize);
                    }
                }
            }
            else
            {
                for (i = 0; i < COUNT; i++)
                {
                    f(dstbuf, srcbuf, size);
                }
            }
            t2 = gettime();
        } while (t2 - t1 < 0.5);
        speed = (double)size * COUNT * loopcount / (t2 - t1) / 1000000.;

        s0 += 1;
        s1 += speed;
        s2 += speed * speed;

        if (speed > maxspeed)
            maxspeed = speed;

        if (s0 > 2)
        {
            s = sqrt((s0 * s2 - s1 * s1) / (s0 * (s0 - 1)));
            if (s < maxspeed / 1000.)
                break;
        }
    }

    if (s / maxspeed * 100. >= 0.1)
    {
        printf("%s%-52s : %8.1f MB/s (%.1f%%)\n", indent_prefix, description,
                                               maxspeed, s / maxspeed * 100.);
    }
    else
    {
        printf("%s%-52s : %8.1f MB/s\n", indent_prefix, description, maxspeed);
    }
    return maxspeed;
}

static void memcpy_wrapper(int64_t *dst, int64_t *src, int size)
{
    memcpy(dst, src, size);
}

static void bandwidth_bench(int64_t *dstbuf, int64_t *srcbuf, int64_t *tmpbuf,
                     int size, int blocksize, const char *indent_prefix)
{
    bench_info *bi = all_bench;

    while (bi->f)
    {
        bandwidth_bench_helper(dstbuf, srcbuf, tmpbuf, size, blocksize,
                               indent_prefix, bi->use_tmpbuf,
                               bi->f,
                               bi->description);
        bi++;
    }

}

int main(int argc, char *argv[])
{
    int64_t *srcbuf, *dstbuf, *tmpbuf;
    void *buf;
    size_t bufsize = SIZE;

    printf("%s: benchmark for memory bandwidth\n", argv[0]);


    buf = alloc_four_aligned_buffers((void **)&srcbuf, bufsize,
                                            (void **)&dstbuf, bufsize,
                                            (void **)&tmpbuf, BLOCKSIZE,
                                            NULL, 0);
    if(!buf)
    {
	    return 0;
    }

    printf("\n");
    printf("================================================================================\n");
    printf("== Memory bandwidth tests                                                     ==\n");
    printf("== Note: If sample standard deviation exceeds 0.1%%, it's shown in brackets.   ==\n");
    printf("================================================================================\n\n");
    
    bandwidth_bench(dstbuf, srcbuf, tmpbuf, bufsize, BLOCKSIZE, " ");
    
    free(buf);

    return 0;
}
