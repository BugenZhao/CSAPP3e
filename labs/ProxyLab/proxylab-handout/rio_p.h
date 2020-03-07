//
// Created by Bugen Zhao on 2020/3/7.
//

#ifndef BUGENCSAPP3E_RIO_P_H
#define BUGENCSAPP3E_RIO_P_H

#include "csapp.h"

void Rio_writenp(int fd, void *usrbuf, size_t n);

ssize_t Rio_readlinebp(rio_t *rp, void *usrbuf, size_t maxlen);

int Open_clientfdp(char *hostname, char *port);

#endif //BUGENCSAPP3E_RIO_P_H
