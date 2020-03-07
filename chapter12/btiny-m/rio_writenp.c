//
// Created by Bugen Zhao on 2020/3/7.
//

#include "csapp.h"

void Rio_writenp(int fd, void *usrbuf, size_t n) {
    if (rio_writen(fd, usrbuf, n) != n) {
        if (errno == EPIPE)
            printf("Broken pipe on connection %d, did the client prematurely close the connection?\n", fd);
        else
            unix_error("Rio_writen error");
    }
}