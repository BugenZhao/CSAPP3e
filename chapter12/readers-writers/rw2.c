//
// Created by Bugen Zhao on 2020/3/6.
//

#include "rw.h"

const char desc[80] = "RW1 - Strong priority to readers";

int readcnt;
int protected;
sem_t *mutex, *w, *wg;

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
        if (readcnt == 0) {
            V(mutex);
            V(w);
        } else V(mutex);
    }
}

void writer() {
    while (1) {
        P(wg);
        P(w);

        // Writing...
        writetimes++;
        if (writetimes % 100 == 0) {
            printf("read/write: %d/%d\n", readtimes, writetimes);
            if (writetimes >= 1000)
                exit(0);
        }

        V(w);
        V(wg);
    }
}

void init() {
    mutex = Sem_open_and_unlink("/bz_mutex_", 1);
    w = Sem_open_and_unlink("/bz_w_", 1);
    wg = Sem_open_and_unlink("/bz_wg_", 1);

    readtimes = 0;
    writetimes = 0;
    readcnt = 0;
    protected = 0;
}

#pragma clang diagnostic pop