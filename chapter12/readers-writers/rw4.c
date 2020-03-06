//
// Created by Bugen Zhao on 2020/3/6.
//

#include "rw.h"

const char desc[80] = "RW2 - Priority to writers";

int writecnt;
sem_t *mutex, *w, *wmutex;

int writetimes;
int readtimes;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void reader() {
    int i = 10000;

    while (i--) {
        if (writecnt > 0) continue;

        P(mutex);
        readtimes++;
        V(mutex);

        // Reading...

    }
}

void writer() {
    while (1) {
        P(wmutex);
        writecnt++;
        V(wmutex);

        P(w);

        // Writing...

        writetimes++;
        if (writetimes % 100 == 0) {
            printf("read/write: %d/%d\n", readtimes, writetimes);
            if (writetimes >= 1000)
                exit(0);
        }

        writecnt--;

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
    wmutex = Sem_open_and_unlink("bz_mutex_", 1);
#endif
    readtimes = 0;
    writetimes = 0;
    writecnt = 0;
}

#pragma clang diagnostic pop