//
// Created by Bugen Zhao on 2020/2/29.
//

#include "csapp.h"

#pragma clang diagnostic ignored "-Wmissing-noreturn"

#define STATIC 1
#define DYNAMIC 0
typedef char string[MAXLINE];
typedef unsigned long long ull;

const char server_name[] = "BTiny Web Server";

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

void get_filetype(string filename, string filetype);

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
    printf("%s is listening on port %s...\n", server_name, port);
    fflush(stdout);
    strcat(open_cmd, port);
    system(open_cmd);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, host, MAXLINE, serv, MAXLINE, 0);
        printf("Accept connection from %s:%s\n", host, serv);
//        fflush(stdout);
        process(connfd);
        Close(connfd);
        printf("Close connection from %s:%s\n\n\n", host, serv);
//        fflush(stdout);
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
        client_error(connfd, method, "501", "Not Implemented", "Not Implemented");
    }
}

void skip_req_headers(rio_t *rp) {
    string buf;
    do {
        Rio_readlineb(rp, buf, MAXLINE);
    } while (strcmp(buf, "\r\n") != 0);
}

int parse_uri(string uri, string filename, string cgiargs) {
    char *ptr;

    if (strstr(uri, "cgi-bin") == NULL) {
        // static
        strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        // default page
        if (filename[strlen(filename) - 1] == '/')
            strcat(filename, "index.html");

        return STATIC;
    } else {
        // dynamic
        ptr = index(uri, '?');
        if (ptr == NULL) strcpy(cgiargs, "");
        else {
            strcpy(cgiargs, ptr + 1);
            *ptr = '\0';
        }
        strcpy(filename, ".");
        strcat(filename, uri);

        return DYNAMIC;
    }
}

void serve_static(int connfd, string filename) {
    struct stat sbuf;
    string filetype;
    string buf;
    int fd;
    char *filebuf;

    printf("Static content\n");

    if (stat(filename, &sbuf) < 0) {
        client_error(connfd, filename, "404", "Not Found", "Not Found");
        return;
    }
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
        client_error(connfd, filename, "403", "Forbidden", "No permission");
        return;
    }

    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: %s\r\n", buf, server_name);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %llu\r\n", buf, sbuf.st_size);
    sprintf(buf, "%sContent-type: %s\r\n", buf, filetype);
    sprintf(buf, "%s\r\n", buf); // IMPORTANT!
    Rio_writen(connfd, buf, strlen(buf));
    printf("%s", buf);

    fd = Open(filename, O_RDONLY, 0);
    filebuf = Mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    Close(fd);
    Rio_writen(connfd, filebuf, sbuf.st_size);
    Munmap(filebuf, sbuf.st_size);
}

void serve_dynamic(int connfd, string filename, string cgiargs) {
    printf("Dynamic content\n");
}

void client_error(int connfd, string cause, string errnum, string msg, string disc) {
    string html, buf;
    ull len;

    printf("Client error %s\n", errnum);

    sprintf(html, "<h1>%s %s</h1><p>%s: %s</p><em>%s</em>", errnum, msg, cause, disc, server_name);
    len = strlen(html);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, msg);
    sprintf(buf, "%sServer: %s\r\n", buf, server_name);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %llu\r\n", buf, len);
    sprintf(buf, "%sContent-type: text/html\r\n", buf);
    sprintf(buf, "%s\r\n", buf); // IMPORTANT!
    Rio_writen(connfd, buf, strlen(buf));
    printf("%s", buf);

    Rio_writen(connfd, html, len);
}

void get_filetype(string filename, string filetype) {
    if (strstr(filename, ".htm"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else
        strcpy(filetype, "text/plain");
}