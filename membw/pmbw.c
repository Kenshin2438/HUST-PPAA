#include <assert.h>
#include <math.h>
#include <memory.h>
#include <pthread.h>
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

void *task(void *arg) {
  const info *data = (info *)arg;

  double t2, t1 = gettime();
  int64_t loopcount = 0;
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

  info **args = (info **)malloc(P * sizeof(info *));
  for (int i = 0; i < P; i++) {
    args[i] = (info *)malloc(sizeof(info));
    args[i]->buf_ = alloc_four_aligned_buffers((void **)(&(args[i]->srcbuf)), SIZE, NULL, 0,
                                               (void **)(&(args[i]->dstbuf)), SIZE, NULL, 0);
    if (args[i] == NULL) return EXIT_FAILURE;
  }
  pthread_t *tester = (pthread_t *)malloc(P * sizeof(pthread_t));
  void **retvals = (void **)malloc(P * sizeof(void *));

  // parallel bandwidth bench
  double speed, maxspeed;
  double s, s0, s1, s2;
  s0 = s1 = s2 = 0;
  maxspeed = 0;

  for (int _ = 0; _ < MAXREPEATS; _++) {
    double t1 = gettime();
    for (int i = 0; i < P; i++) {
      pthread_create(&tester[i], NULL, task, (void *)args[i]);
    }
    for (int i = 0; i < P; i++) pthread_join(tester[i], &retvals[i]);
    double t2 = gettime();

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
  for (int i = 0; i < P; i++) free(args[i]->buf_);
  free(args);

  return EXIT_SUCCESS;
}
