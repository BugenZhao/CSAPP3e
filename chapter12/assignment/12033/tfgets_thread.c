//
// Created by Bugen Zhao on 2020/3/6.
//


/*
 * An fgets() with timeout, based on threads.
 * See 08025, 12031, 12032 for other implementations.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "csapp.h"

#define TIMEOUT 5
#define SIZE 80

static char *ret;

typedef struct {
    char *s;
    int size;
    FILE *stream;
} args_t;

void fgets_thread(void *vargp) {
    args_t args = *(args_t *) vargp;
    ret = fgets(args.s, args.size, args.stream);
}

void sleep_thread(void *vargp) {
    pthread_t fgets_tid = *(pthread_t *) vargp;
    Pthread_detach(Pthread_self());
    Sleep(TIMEOUT);
    Pthread_cancel(fgets_tid);
}

char *tfgets(char *s, int size, FILE *stream) {
    pthread_t fgets_tid, sleep_tid;
    args_t args = {s, size, stream};
    ret = NULL;

    Pthread_create(&fgets_tid, NULL, (void *(*)(void *)) fgets_thread, &args);
    Pthread_create(&sleep_tid, NULL, (void *(*)(void *)) sleep_thread, &fgets_tid);
    Pthread_join(fgets_tid, NULL);

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