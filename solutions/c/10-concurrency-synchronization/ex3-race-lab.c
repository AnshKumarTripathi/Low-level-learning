/* Solution: C 10 Exercise 3 — Race condition lab. Attempt the exercise first.
 * Usage: race --racy | --mutex | --atomic       Build with -pthread -O0 (see below).
 *
 * --racy is a deliberate data race (undefined behavior). Build it with -O0: at -O2
 * GCC may turn each thread's loop into one addition, which hides the race.
 */
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define THREADS 4
#define ITERATIONS 1000000

static long counter_racy = 0;                      /* shared, unprotected: the bug */
static long counter_locked = 0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static atomic_long counter_atomic = 0;

static void *work_racy(void *arg)
{
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        counter_racy++;                            /* load, add, store: can interleave */
    }
    return NULL;
}

static void *work_mutex(void *arg)
{
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        counter_locked++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

static void *work_atomic(void *arg)
{
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_fetch_add(&counter_atomic, 1);      /* one indivisible step */
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    void *(*work)(void *) = NULL;
    if (argc == 2 && strcmp(argv[1], "--racy") == 0) {
        work = work_racy;
    } else if (argc == 2 && strcmp(argv[1], "--mutex") == 0) {
        work = work_mutex;
    } else if (argc == 2 && strcmp(argv[1], "--atomic") == 0) {
        work = work_atomic;
    }
    if (work == NULL) {
        fprintf(stderr, "usage: race --racy | --mutex | --atomic\n");
        return 2;
    }

    struct timespec t0, t1;
    timespec_get(&t0, TIME_UTC);
    pthread_t tid[THREADS];
    for (int t = 0; t < THREADS; t++) {
        if (pthread_create(&tid[t], NULL, work, NULL) != 0) {
            fprintf(stderr, "error: cannot start thread\n");
            return 1;
        }
    }
    for (int t = 0; t < THREADS; t++) {
        pthread_join(tid[t], NULL);
    }
    timespec_get(&t1, TIME_UTC);

    long result = (work == work_racy) ? counter_racy
                : (work == work_mutex) ? counter_locked
                : atomic_load(&counter_atomic);
    long expected = (long)THREADS * ITERATIONS;
    double ms = (double)(t1.tv_sec - t0.tv_sec) * 1000.0 + (double)(t1.tv_nsec - t0.tv_nsec) / 1e6;
    printf("count=%ld expected=%ld %s (%.1f ms)\n", result, expected,
           result == expected ? "correct" : "LOST UPDATES", ms);
    return 0;
}
