//
// Created by Bugen Zhao on 2020/3/6.
//

#include "rw.h"

const char desc[80] = "Weak priority to readers";

int readcnt;
sem_t *mutex, *w;

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
        if (readcnt == 1) P(w);
        V(mutex);

        // Reading...

        P(mutex);
        readcnt--;
        if (readcnt == 0) V(w);
        V(mutex);
    }
}

void writer() {
    while (1) {
        P(w);

        // Writing...
        writetimes++;
        if (writetimes % 100 == 0) {
            printf("read/write: %d/%d\n", readtimes, writetimes);
            if (writetimes >= 1000)
                exit(0);
        }

        V(w);
    }
}

void init() {
#ifndef BUGENCSAPP3E_BZCSAPP_H
    // mode 0600 is critical!!
    mutex = sem_open("/bz_mutex_", O_CREAT | O_EXCL, 0600, 1);
    w = sem_open("/bz_w_", O_CREAT | O_EXCL, 0600, 1);

    // IMMEDIATELY unlink the semaphores to make sure they will be closed (sem_close) correctly
    //  after program exits.
    sem_unlink("/bz_mutex_");
    sem_unlink("/bz_w_");
#else
    mutex = Sem_open_and_unlink("/bz_mutex_", 1);
    w = Sem_open_and_unlink("/bz_w_", 1);
#endif
    readtimes = 0;
    writetimes = 0;
    readcnt = 0;
}

#pragma clang diagnostic pop