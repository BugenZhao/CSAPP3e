//
// Created by Bugen Zhao on 2020/3/6.
//

#ifndef BUGENCSAPP3E_BZCSAPP_H
#define BUGENCSAPP3E_BZCSAPP_H

#include <semaphore.h>

sem_t * Sem_open_and_unlink(const char *name, unsigned int value);

int Sem_close(sem_t *sem);

#endif //BUGENCSAPP3E_BZCSAPP_H
