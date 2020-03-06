//
// Created by Bugen Zhao on 2020/3/6.
//

#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT 10
#define TIMES 10000

sem_t *sem;
int bz_count = 0;
const char *SEM_NAME = "/test_sem0";

void worker() {
    int i = TIMES;

    while (i--) {
        sem_wait(sem);
        ++bz_count;
        sem_post(sem);
    }
}

int main() {
    pthread_t tid[THREAD_COUNT];
    int i;

    sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 1);
    if (sem == SEM_FAILED) {
        fprintf(stderr, "semaphore open failed: %s", strerror(errno));
        exit(errno);
    }

    for (i = 0; i < THREAD_COUNT; ++i) {
        pthread_create(tid + i, NULL, worker, NULL);
    }

    for (i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(tid[i], NULL);
    }

    if (sem_unlink(SEM_NAME) != 0) {
        fprintf(stderr, "semaphore unlink failed: %s", strerror(errno));
        exit(errno);
    }
    if (sem_close(sem) != 0) {
        fprintf(stderr, "semaphore close failed: %s", strerror(errno));
        exit(errno);
    }

    printf("bz_count = %d\n", bz_count);
    exit(0);
}