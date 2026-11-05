/* Solution: C 10 Exercise 2 — Bounded work queue (producer/consumer). Attempt the exercise first.
 * Usage: workq --threads <n> --jobs <m>        Build with -pthread.
 * Each job counts the vowels in a generated string; the total proves no job was lost.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_CAP 8
#define MAX_THREADS 64

typedef struct {
    int jobs[QUEUE_CAP];          /* ring buffer of job ids */
    size_t head, tail, count;
    int closed;                   /* set by the producer when no more jobs will come */
    size_t max_depth;
    long long jobs_done;
    long long vowels_total;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Queue;

static void queue_push(Queue *q, int job)
{
    pthread_mutex_lock(&q->lock);
    while (q->count == QUEUE_CAP) {                 /* always a loop: re-check after waking */
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    q->jobs[q->tail] = job;
    q->tail = (q->tail + 1) % QUEUE_CAP;
    q->count++;
    if (q->count > q->max_depth) {
        q->max_depth = q->count;
    }
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

/* Returns 1 and sets *job, or 0 when the queue is closed and empty. */
static int queue_pop(Queue *q, int *job)
{
    pthread_mutex_lock(&q->lock);
    while (q->count == 0 && !q->closed) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    if (q->count == 0) {                            /* closed and drained */
        pthread_mutex_unlock(&q->lock);
        return 0;
    }
    *job = q->jobs[q->head];
    q->head = (q->head + 1) % QUEUE_CAP;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return 1;
}

static void queue_close(Queue *q)
{
    pthread_mutex_lock(&q->lock);
    q->closed = 1;
    pthread_cond_broadcast(&q->not_empty);          /* wake EVERY consumer so each can exit */
    pthread_mutex_unlock(&q->lock);
}

static int count_vowels_for_job(int job)
{
    char text[64];
    snprintf(text, sizeof text, "job number %d is here", job);
    int n = 0;
    for (const char *p = text; *p != '\0'; p++) {
        if (strchr("aeiou", *p) != NULL) {
            n++;
        }
    }
    return n;
}

static void *consumer(void *arg)
{
    Queue *q = arg;
    int job;
    while (queue_pop(q, &job)) {
        int v = count_vowels_for_job(job);          /* the work happens outside the lock */
        pthread_mutex_lock(&q->lock);
        q->jobs_done++;
        q->vowels_total += v;
        pthread_mutex_unlock(&q->lock);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    long threads = 0, jobs = -1;
    if (argc == 5 && strcmp(argv[1], "--threads") == 0 && strcmp(argv[3], "--jobs") == 0) {
        threads = strtol(argv[2], NULL, 10);
        jobs = strtol(argv[4], NULL, 10);
    }
    if (threads < 1 || threads > MAX_THREADS || jobs < 0 || jobs > 100000000) {
        fprintf(stderr, "usage: workq --threads <1..%d> --jobs <0..100000000>\n", MAX_THREADS);
        return 2;
    }

    Queue q;
    memset(&q, 0, sizeof q);
    pthread_mutex_init(&q.lock, NULL);
    pthread_cond_init(&q.not_empty, NULL);
    pthread_cond_init(&q.not_full, NULL);

    pthread_t tid[MAX_THREADS];
    long started = 0;
    for (; started < threads; started++) {
        if (pthread_create(&tid[started], NULL, consumer, &q) != 0) {
            fprintf(stderr, "error: cannot start thread\n");
            break;
        }
    }
    if (started > 0) {
        for (long j = 0; j < jobs; j++) {           /* main is the producer */
            queue_push(&q, (int)j);
        }
    }
    queue_close(&q);
    for (long t = 0; t < started; t++) {
        pthread_join(tid[t], NULL);
    }

    long long expected = 0;
    for (long j = 0; j < jobs; j++) {               /* single-threaded reference */
        expected += count_vowels_for_job((int)j);
    }
    printf("jobs_done=%lld\n", q.jobs_done);
    printf("max_queue_depth=%zu\n", q.max_depth);
    printf("vowels=%lld (expected %lld)\n", q.vowels_total, expected);

    pthread_cond_destroy(&q.not_full);
    pthread_cond_destroy(&q.not_empty);
    pthread_mutex_destroy(&q.lock);
    return (started == threads && q.jobs_done == jobs && q.vowels_total == expected) ? 0 : 1;
}
