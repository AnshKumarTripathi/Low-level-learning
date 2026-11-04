/* Solution: Exercise I1 — Distance between points. Attempt the exercise first. */
#include <stdio.h>

struct Point {
    int x;
    int y;
};

int dist2(struct Point a, struct Point b)
{
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return dx * dx + dy * dy;
}

int main(void)
{
    struct Point p, q;
    if (scanf("%d %d %d %d", &p.x, &p.y, &q.x, &q.y) != 4) {
        fprintf(stderr, "error: expected x1 y1 x2 y2\n");
        return 1;
    }
    printf("dist2=%d\n", dist2(p, q));
    return 0;
}
