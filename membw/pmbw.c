#define _GNU_SOURCE /* See feature_test_macros(7) */

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#ifdef __linux__
  #include <fcntl.h>
  #include <linux/fb.h>
  #include <sys/mman.h>
  #include <unistd.h>
#endif

#include "util.h"

#define SIZE (32 * 1024 * 1024)
#define BLOCKSIZE 2048
#define COUNT 16
#define MAXREPEATS 10

typedef struct {
  void *buf_;
  int64_t *srcbuf;
  int64_t *dstbuf;
} info;

static pthread_barrier_t barrier;

void *task(void *arg) {
  const info *data = (info *)arg;
  double t2, t1;
  int64_t loopcount = 0;

  pthread_barrier_wait(&barrier);

  t1 = gettime();
  do {
    loopcount++;
    for (int i = 0; i < COUNT; i++) {
      memcpy(data->dstbuf, data->srcbuf, SIZE);
    }
    t2 = gettime();
  } while (t2 - t1 < 0.5);
  return (void *)loopcount;
}

int main(int argc, char *argv[]) {
  assert(argc == 2 && "Plz input the thread number.\n");
  const int P = atoi(argv[1]);

#ifdef COURSE_CUNOK
  cpu_set_t *cpuset = (cpu_set_t *)malloc(P * sizeof(cpu_set_t));
  for (int i = 0; i < P; i++) {
    const int id = 14 * (i % 4) + ((i % 4) >= 2 ? 13 - (i / 4) : (i / 4));
    CPU_ZERO(&cpuset[i]);
    CPU_SET(id, &cpuset[i]);
  }
#endif

  pthread_t *tester = (pthread_t *)malloc(P * sizeof(pthread_t));
  void **retvals = (void **)malloc(P * sizeof(void *));

  info **args = (info **)malloc(P * sizeof(info *));
  for (int i = 0; i < P; i++) {
#ifdef COURSE_CUNOK
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset[i]);
#endif
    args[i] = (info *)malloc(sizeof(info));
    args[i]->buf_ = alloc_four_aligned_buffers((void **)(&(args[i]->srcbuf)), SIZE, NULL, 0,
                                               (void **)(&(args[i]->dstbuf)), SIZE, NULL, 0);
    if (args[i] == NULL) return EXIT_FAILURE;
    memcpy(args[i]->dstbuf, args[i]->srcbuf, SIZE);
  }

  // parallel bandwidth bench
  double t1, t2;
  double speed, maxspeed;
  double s, s0, s1, s2;
  s0 = s1 = s2 = 0;
  maxspeed = 0;

  for (int _ = 0; _ < MAXREPEATS; _++) {
    pthread_barrier_init(&barrier, NULL, P);
    for (int i = 0; i < P; i++) {
#ifdef COURSE_CUNOK
      sched_setaffinity(0, sizeof(cpu_set_t), &cpuset[i]);
#endif
      pthread_create(&tester[i], NULL, task, (void *)args[i]);
      if (i == P - 1) t1 = gettime();
    }
    for (int i = 0; i < P; i++) pthread_join(tester[i], &retvals[i]);
    t2 = gettime();
    pthread_barrier_destroy(&barrier);

    int64_t loops = 0;
    for (int i = 0; i < P; i++) loops += (int64_t)retvals[i];
    speed = (double)SIZE * COUNT * loops / (t2 - t1) / 1000000.;

    s0 += 1;
    s1 += speed;
    s2 += speed * speed;

    if (speed > maxspeed) maxspeed = speed;

    if (s0 > 2) {
      s = sqrt((s0 * s2 - s1 * s1) / (s0 * (s0 - 1)));
      if (s < maxspeed / 1000.) break;
    }
  }

  if (s / maxspeed * 100. >= 0.1) {
    printf("pmbw (%d threads) : %8.1f MB/s (%.1f%%)\n", P, maxspeed, s / maxspeed * 100.);
  } else {
    printf("pmbw (%d threads) : %8.1f MB/s\n", P, maxspeed);
  }

#ifdef COURSE_CUNOK
  free(cpuset);
#endif
  free(tester);
  free(retvals);
  for (int i = 0; i < P; i++) free(args[i]->buf_);
  free(args);

  return EXIT_SUCCESS;
}
