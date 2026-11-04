/* Solution: Exercise D1 — Letter grade. Attempt the exercise first. */
#include <stdio.h>

int main(void)
{
    int score;
    if (scanf("%d", &score) != 1) {
        fprintf(stderr, "error: not a number\n");
        return 1;
    }
    if (score < 0 || score > 100) {
        fprintf(stderr, "error: score must be 0..100\n");
        return 1;
    }

    char grade;
    if (score >= 90) {
        grade = 'A';
    } else if (score >= 80) {
        grade = 'B';
    } else if (score >= 70) {
        grade = 'C';
    } else if (score >= 60) {
        grade = 'D';
    } else {
        grade = 'F';
    }
    printf("grade=%c\n", grade);
    return 0;
}
