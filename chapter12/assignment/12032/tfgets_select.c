//
// Created by Bugen Zhao on 2020/3/6.
//


/*
 * An fgets() with timeout, based on I/O multiplexing.
 * See 08025, 12031 for other implementations.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define TIMEOUT 5
#define SIZE 80

char *tfgets(char *s, int size, FILE *stream) {
    struct timeval tv = {5, 0};
    fd_set fds;
    int ret;

    FD_ZERO(&fds);
    FD_SET(fileno(stream), &fds);

    ret = select(fileno(stream) + 1, &fds, NULL, NULL, &tv);
    if (ret) return fgets(s, size, stream);
    else return NULL;
}

int main() {
    char buffer[SIZE];
    printf("You have %d secs to input:\n", TIMEOUT);
    char *result = tfgets(buffer, SIZE, stdin);
    if (result) puts(result);
    else puts("time out or EOF");
    exit(0);
}