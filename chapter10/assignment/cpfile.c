//
// Created by Bugen Zhao on 2020/2/27.
//

#include "csapp.h"

int main(int argc, char **argv) {
    int n;
    char buf[MAXLINE];
    rio_t rio;
    int fd = STDIN_FILENO;

    if (argc >= 2)
        fd = Open(argv[1], O_RDONLY, 0);

    Rio_readinitb(&rio, fd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
        Rio_writen(STDOUT_FILENO, buf, n);

    Close(fd);
    exit(0);
}