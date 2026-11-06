/* Solution: C 08 Exercise 3 — run command wrapper. Attempt the exercise first.
 * Usage: run <program> [args...]
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

/* Runs argv[0] with arguments argv (NULL-terminated).
 * Returns the exit code, -1 if the program was not found, -2 on other failures. */
static int run_and_wait(char *const argv[])
{
#ifdef _WIN32
    intptr_t code = _spawnvp(_P_WAIT, argv[0], (const char *const *)argv);
    if (code == -1) {
        return (errno == ENOENT) ? -1 : -2;
    }
    return (int)code;
#else
    pid_t pid = fork();
    if (pid < 0) {
        return -2;
    }
    if (pid == 0) {
        execvp(argv[0], argv);
        _exit(127);                              /* exec failed: 127 by convention */
    }
    int status;
    if (waitpid(pid, &status, 0) < 0 || !WIFEXITED(status)) {
        return -2;
    }
    int code = WEXITSTATUS(status);
    return (code == 127) ? -1 : code;
#endif
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage: run <program> [args...]\n");
        return 2;
    }
    fflush(stdout);                              /* keep our output and the child's in order */
    int code = run_and_wait(argv + 1);           /* argv[argc] is NULL, as the API requires */
    if (code == -1) {
        fprintf(stderr, "error: program not found: %s\n", argv[1]);
        return 127;
    }
    if (code < 0) {
        fprintf(stderr, "error: could not run %s\n", argv[1]);
        return 1;
    }
    printf("exit_code=%d\n", code);
    return 0;
}
