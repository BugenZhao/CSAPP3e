//
// Created by Bugen Zhao on 2020/3/6.
//

#include "rw.h"

int main() {
    int i;
    pthread_t tid;

    init();

    for (i = 0; i < 5; i++) {
        if (i % 5 == 0)
            Pthread_create(&tid, NULL, (void *(*)(void *)) writer, NULL);
        else
            Pthread_create(&tid, NULL, (void *(*)(void *)) reader, NULL);
    }


    printf("%s\n", desc);
    Pthread_exit(NULL);

    exit(0);
}