//
// Created by Bugen Zhao on 2020/3/6.
//

/*
 * unix > telnet localhost <port>
 * ^]
 * telnet > mode char
 * Hello, ^]
 * telnet > mode line
 * world!<ret>
 * Hello, world!
 * Connection closed by foreign host.
 */

#include "csapp.h"

int echo_line(int connfd);

void command(void);

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    fd_set read_set, ready_set;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(argv[1]);

    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);
    FD_SET(listenfd, &read_set);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        ready_set = read_set;
        Select(listenfd + 1, &ready_set, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &ready_set))
            command();
        if (FD_ISSET(listenfd, &ready_set)) {
            clientlen = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
            echo_line(connfd);
            Close(connfd);
        }
    }
#pragma clang diagnostic pop
}

void command(void) {
    char buf[MAXLINE];
    if (!Fgets(buf, MAXLINE, stdin))
        exit(0); /* EOF */
    printf("%s", buf); /* Process the input command */
}

int echo_line(int connfd) {
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    if ((n = Rio_readlineb(&rio, buf, MAXLINE)) >= 0) {
        printf("server received %d bytes\n", (int) n);
        Rio_writen(connfd, buf, n);
        return n;
    }
    return -1;
}