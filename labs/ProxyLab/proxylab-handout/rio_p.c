//
// Created by Bugen Zhao on 2020/3/7.
//

#include "rio_p.h"

void Rio_writenp(int fd, void *usrbuf, size_t n) {
    if (rio_writen(fd, usrbuf, n) != n) {
        if (errno == EPIPE)
            fprintf(stderr, "Broken pipe on connection %d, did the client prematurely close the connection?\n", fd);
        else
            unix_error("Rio_writen error");
    }
}

ssize_t Rio_readlinebp(rio_t *rp, void *usrbuf, size_t maxlen) {
    ssize_t rc;
    if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0)
        fprintf(stderr, "Rio_readlinebp error, did the client prematurely close the connection?\n");
    return rc;
}

int Open_clientfdp(char *hostname, char *port) {
    int rc;

    if ((rc = open_clientfd(hostname, port)) < 0)
        fprintf(stderr, "Open_clientfdp error, are we able to reach the address?\n");
    return rc;
}