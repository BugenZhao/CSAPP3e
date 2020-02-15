//
// Created by Bugen Zhao on 2020/2/15.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define N 6

int main() {
    int status, i;
    pid_t pid;
    char *ptr = NULL;
    char desc[80];

    for (i = 0; i < N; ++i) {
        if (fork() == 0) {
            if (i == 0) {
                printf("child %d: Waiting for a signal...\n", getpid());
                fflush(stdout);
                pause();
            } else if (i % 2) {
                ptr[0] = 0; // Segmentation fault
            } else exit(i);
        }
    }

    while ((pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status))
            printf("child %d terminated normally with exit status=%d\n", pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status)) {
            sprintf(desc, "child %d terminated by signal %d", pid, WTERMSIG(status));
            psignal(WTERMSIG(status), desc);
        } else
            fprintf(stderr, "child %d terminated abnormally\n", pid);
    }

    if (errno != ECHILD) fprintf(stderr, "waitpid error");

    exit(0);
}