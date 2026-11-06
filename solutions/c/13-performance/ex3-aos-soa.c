/* Solution: C 13 Exercise 3 — Array of structs vs struct of arrays. Attempt the exercise first.
 * Build optimised: gcc -std=c17 -O2 -Wall -Wextra ex3-aos-soa.c -o aos_soa
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COUNT 1000000
#define STEPS 100

struct Particle {
    float x, y, z, vx, vy, vz, mass, charge;        /* 32 bytes */
};

struct Particles {
    float *x, *y, *z, *vx, *vy, *vz, *mass, *charge;
};

static double now_ms(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}

static void move_aos(struct Particle *p, float dt)
{
    for (size_t i = 0; i < COUNT; i++) {             /* uses 12 of every 32 bytes loaded... */
        p[i].x += p[i].vx * dt;
        p[i].y += p[i].vy * dt;
        p[i].z += p[i].vz * dt;
    }
}

static void move_soa(struct Particles *p, float dt)
{
    for (size_t i = 0; i < COUNT; i++) {             /* ...here every byte loaded is used */
        p->x[i] += p->vx[i] * dt;
        p->y[i] += p->vy[i] * dt;
        p->z[i] += p->vz[i] * dt;
    }
}

int main(void)
{
    struct Particle *aos = malloc(COUNT * sizeof *aos);
    float *block = malloc(8 * (size_t)COUNT * sizeof *block);
    if (aos == NULL || block == NULL) {
        free(aos);
        free(block);
        return 1;
    }
    struct Particles soa = {block, block + COUNT, block + 2 * COUNT, block + 3 * COUNT,
                            block + 4 * COUNT, block + 5 * COUNT, block + 6 * COUNT, block + 7 * COUNT};
    for (size_t i = 0; i < COUNT; i++) {
        float v = (float)(i % 100) / 100.0f;
        aos[i] = (struct Particle){0, 0, 0, v, v, v, 1, 1};
        soa.x[i] = soa.y[i] = soa.z[i] = 0;
        soa.vx[i] = soa.vy[i] = soa.vz[i] = v;
        soa.mass[i] = soa.charge[i] = 1;
    }

    double t0 = now_ms();
    for (int s = 0; s < STEPS; s++) {
        move_aos(aos, 0.01f);
    }
    double t1 = now_ms();
    for (int s = 0; s < STEPS; s++) {
        move_soa(&soa, 0.01f);
    }
    double t2 = now_ms();

    printf("aos=%.1f ms soa=%.1f ms (check: %.3f vs %.3f)\n", t1 - t0, t2 - t1,
           (double)aos[COUNT - 1].x, (double)soa.x[COUNT - 1]);
    free(aos);
    free(block);
    return 0;
}
