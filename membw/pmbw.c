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

// clang-format off
#define SIZE             (32 * 1024 * 1024)
#define BLOCKSIZE        2048
#define COUNT            16
#define MAXREPEATS       10

static pthread_barrier_t barrier;

void *task(void *arg) {
  const size_t id = (size_t)arg;

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(id, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

  void *buf;
  int64_t *srcbuf, *dstbuf;
  buf = alloc_four_aligned_buffers((void **)(&srcbuf), SIZE, NULL, 0,
                                   (void **)(&dstbuf), SIZE, NULL, 0);
  if (buf == NULL) exit(-1);
  memcpy(dstbuf, srcbuf, SIZE);

  double t2, t1;
  int64_t loopcount = 0;

  pthread_barrier_wait(&barrier);
  t1 = gettime();
  do {
    loopcount++;
    for (int i = 0; i < COUNT; i++) memcpy(dstbuf, srcbuf, SIZE);
    t2 = gettime();
  } while (t2 - t1 < 0.5);
  free(buf);
  return (void *)loopcount;
}

static size_t get_cpu_id(size_t thread_id) {
  static const size_t SOCKET_NUM       =  4;
  static const size_t CORES_PRE_SOCKET = 14;
  // NUMA node 0 cpus: 0-13       28-41
  // NUMA node 1 cpus:      14-27       42-55
  const size_t socket_id = thread_id % SOCKET_NUM;
  const size_t core_id   = thread_id / SOCKET_NUM;
  return socket_id * CORES_PRE_SOCKET + (socket_id < 2 ? core_id : CORES_PRE_SOCKET - 1 - core_id);
}
// clang-format on

int main(int argc, char *argv[]) {
  assert(argc == 2 && "Plz input the thread number.\n");
  const int P = atoi(argv[1]);

  pthread_t *tester = (pthread_t *)malloc(P * sizeof(pthread_t));
  void **retvals = (void **)malloc(P * sizeof(void *));

  // parallel bandwidth bench
  double t1, t2;
  double speed, maxspeed;
  double s, s0, s1, s2;
  s0 = s1 = s2 = 0;
  maxspeed = 0;

  for (int _ = 0; _ < MAXREPEATS; _++) {
    pthread_barrier_init(&barrier, NULL, P + 1);
    for (int i = 0; i < P; i++) {
      const size_t id = get_cpu_id(i);
      pthread_create(&tester[i], NULL, task, (void *)id);
    }
    pthread_barrier_wait(&barrier);
    t1 = gettime();
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

  free(tester);
  free(retvals);

  return 0;
}
