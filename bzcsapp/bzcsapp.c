//
// Created by Bugen Zhao on 2020/3/6.
//

#include <csapp.h>
#include "bzcsapp.h"

sem_t *Sem_open_and_unlink(const char *name, unsigned int value) {
    sem_t *sem;
    if ((sem = sem_open(name, O_CREAT | O_EXCL, 0600, value)) < 0)
        unix_error("Semaphore open failed");
    if(sem_unlink(name)<0)
        unix_error("Semaphore unlink failed");
    return sem;
}