//
// Created by Bugen Zhao on 2020/3/6.
//

/*
 * Here is an elegant solution due to Henri Casanova.
 * The idea is to using a counting semaphore initialized to N .
 * Each reader must acquire 1 resource to be able to read, thus N concurrent readers are allowed.
 * Similarly, each writer must acquire N resources to be able to write,
 * and therefore only writer can be executing at a time,
 * and when a writer is executing, no other readers can be executing.
 * A mutex in the writer ensures that only one writer at a time is busy accumulating resources.
 */

#include "rw.h"

#define N 3

const char desc[80] = "RW1 - No more than N concurrent readers";

int readcnt;
sem_t *mutex, *sem, *wmutex;

int writetimes;
int readtimes;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void reader() {
    int i = 10000;

    while (i--) {
        P(mutex);
        readcnt++;
        readtimes++;
        if (readcnt == 1) P(sem);
        V(mutex);

        // Reading...

        P(mutex);
        readcnt--;
        if (readcnt == 0) {
            V(mutex);
            V(sem);
        } else V(mutex);
    }
}

void writer() {
    int i;

    while (1) {
        P(wmutex);
        for (i = 0; i < N; ++i) P(sem);
        V(wmutex);

        // Writing...
        writetimes++;
        if (writetimes % 100 == 0) {
            printf("read/write: %d/%d\n", readtimes, writetimes);
            if (writetimes >= 1000)
                exit(0);
        }

        for (i = 0; i < N; ++i) V(sem);
    }
}

void init() {
    mutex = Sem_open_and_unlink("/bz_mutex_", 1);
    sem = Sem_open_and_unlink("/bz_w_", N);
    wmutex = Sem_open_and_unlink("/bz_wmutex_", 1);

    readtimes = 0;
    writetimes = 0;
    readcnt = 0;
}

#pragma clang diagnostic pop