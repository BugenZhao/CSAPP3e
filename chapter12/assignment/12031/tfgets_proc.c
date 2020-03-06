//
// Created by Bugen Zhao on 2020/3/6.
//

/*
 * An fgets() with timeout, based on processes.
 * See 08025 for an implementation based on alarm signal.
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>

#define TIMEOUT 5
#define SIZE 80

sigjmp_buf buf;

void sigchld_handler(int sig) {
    wait(NULL);
    longjmp(buf, 1);
}

char *tfgets(char *s, int size, FILE *stream) {
    pid_t pid;
    char *ret = NULL;

    if ((pid = fork()) == 0) {
        // Child: send SIGCHLD after TIMEOUT seconds
        sleep(TIMEOUT);
        exit(0);
    } else { // Parent
        if (!sigsetjmp(buf, 0)) {
            // First time
            signal(SIGCHLD, sigchld_handler);
            ret = fgets(s, size, stream);
            kill(pid, SIGKILL);
            pause();
        }
        // Jump
        return ret;
    }
}

int main() {
    char buffer[SIZE];
    printf("You have %d secs to input:\n", TIMEOUT);
    char *result = tfgets(buffer, SIZE, stdin);
    if (result) puts(result);
    else puts("time out or EOF");
    exit(0);
}