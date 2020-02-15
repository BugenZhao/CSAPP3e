//
// Created by Bugen Zhao on 2020/2/15.
//

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>

#define TIMEOUT 5
#define SIZE 80

sigjmp_buf buf;

void handler(int sig) {
    longjmp(buf, 1);
}

char *tfgets(char *s, int size, FILE *stream) {
    char *ret = NULL;
    if (!setjmp(buf)) {
        signal(SIGALRM, handler);
        alarm(TIMEOUT);
        ret = fgets(s, size, stream);
    } else {
        ret = NULL;
    }
    signal(SIGALRM, SIG_DFL);
    return ret;
}

int main() {
    char buffer[SIZE];
    printf("You have %d secs to input:\n", TIMEOUT);
    char *result = tfgets(buffer, SIZE, stdin);
    if (result) puts(result);
    else puts("time out or EOF");
    exit(0);
}