//
// Created by Bugen Zhao on 2020/3/6.
//

#include "csapp.h"

int echo_line(int connfd);

void command(void);

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    fd_set read_set, ready_set;
    int max_fd = 0;
    int fd;
    int read_cnt;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(argv[1]);

    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);
    FD_SET(listenfd, &read_set);
    max_fd = listenfd;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        ready_set = read_set;

        // CRITICAL: select only examine fd in [0, n - 1]
        Select(max_fd + 1, &ready_set, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &ready_set))
            command();
        if (FD_ISSET(listenfd, &ready_set)) {
            clientlen = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
            if (connfd >= FD_SETSIZE) {
                fprintf(stderr, "Too many clients!\n");
                Close(connfd);
            } else {
                // add new client, put its connection fd to the read_set
                max_fd = connfd > max_fd ? connfd : max_fd;
                FD_SET(connfd, &read_set);  //! Read Set
            }
        }

        // Serve clients which are already existed and ready to read
        for (fd = listenfd + 1; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &ready_set)) { //! Ready Set
                read_cnt = echo_line(fd);
                if (read_cnt <= 0) {
                    printf("Get return value: %d, close connection %d...\n", read_cnt, fd);
                    Close(fd);
                    FD_CLR(fd, &read_set);
                }
            }

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
    if ((n = Rio_readlineb(&rio, buf, MAXLINE)) > 0) {
        printf("Server received %d bytes from connection %d: %s", (int) n, connfd, buf);
        Rio_writen(connfd, buf, n);
        return n;
    }
    return -1;
}