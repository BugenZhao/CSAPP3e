//
// Created by Bugen Zhao on 2020/2/29.
//

#include "csapp.h"

#pragma clang diagnostic ignored "-Wmissing-noreturn"

typedef char string[MAXLINE];

void bye(int sig) {
    printf("Bye\n");
    exit(0);
}

void process(int connfd);

void skip_req_headers(rio_t *rp);

int parse_uri(string uri, string filename, string cgiargs);

void serve_static(int connfd, string filename);

void serve_dynamic(int connfd, string filename, string cgiargs);

void client_error(int connfd, string cause, string errnum, string msg, string disc);

int main(int argc, char **argv) {
    int listenfd, connfd;
    char port[6];
    string host, serv;
    struct sockaddr_storage clientaddr;
    socklen_t clientlen;
    string open_cmd = "open http://localhost:";

    if (signal(SIGINT, bye) == SIG_ERR)
        unix_error("signal error");

    if (argc >= 2)
        strncpy(port, argv[1], 5);
    else
        strcpy(port, "1018");

    listenfd = open_listenfd(port);
    printf("BTiny is listening on port %s...\n", port);
    fflush(stdout);
    strcat(open_cmd, port);
    system(open_cmd);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, host, MAXLINE, serv, MAXLINE, 0);
        printf("Accept connection from %s:%s\n", host, serv);
        fflush(stdout);
        process(connfd);
        Close(connfd);
        printf("Close connection from %s:%s\n", host, serv);
        fflush(stdout);
    }
}

void process(int connfd) {
    string buf;
    string method, uri, version;
    string filename, cgiargs;
    rio_t rio;
    int is_static;

    Rio_readinitb(&rio, connfd);
    // get HTTP header / request line
    if (Rio_readlineb(&rio, buf, MAXLINE) == 0) return;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);

    if (strcasecmp(method, "GET") == 0) {
        skip_req_headers(&rio);
        // serve GET request
        is_static = parse_uri(uri, filename, cgiargs);
        if (is_static) serve_static(connfd, filename);
        else serve_dynamic(connfd, filename, cgiargs);
    } else {
        client_error(connfd, method, "501", "Not Implemented", "Sorry");
    }
}

void skip_req_headers(rio_t *rp) {
    string buf;
    do {
        Rio_readlineb(rp, buf, MAXLINE);
    } while (strcmp(buf, "\r\n") != 0);
}

int parse_uri(string uri, string filename, string cgiargs) {
    return 1;
}

void serve_static(int connfd, string filename) {

}

void serve_dynamic(int connfd, string filename, string cgiargs) {

}

void client_error(int connfd, string cause, string errnum, string msg, string disc) {

}