//
// Created by Bugen Zhao on 2020/3/6.
//

#include "rw.h"

const char desc[80] = "RW2 - Priority to writers - Better";

int writecnt;
int readcnt;
sem_t *wcmutex, *rcmutex, *w, *r;

int writetimes;
int readtimes;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void reader() {
    int i = 10000;

    while (i--) {
        P(r);
        P(rcmutex);
        readcnt++;
        if (readcnt == 1) P(w);
        V(rcmutex);

        readtimes++;

        V(r);

        // Reading...

        P(rcmutex);
        readcnt--;
        if (readcnt == 0) V(w);
        V(rcmutex);
    }
}

void writer() {
    while (1) {
        P(wcmutex);
        writecnt++;
        if (writecnt == 1) P(r); // do not read anymore
        V(wcmutex);

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

        P(wcmutex);
        writecnt--;
        if (writecnt == 0) V(r);
        V(wcmutex);
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
    rcmutex = Sem_open_and_unlink("/bz_rc_mutex_", 1);
    wcmutex = Sem_open_and_unlink("bz_wc_mutex_", 1);
    r = Sem_open_and_unlink("/bz_r_", 1);
    w = Sem_open_and_unlink("/bz_w_", 1);
#endif
    readtimes = 0;
    writetimes = 0;
    readcnt = 0;
    writecnt = 0;
}

#pragma clang diagnostic pop