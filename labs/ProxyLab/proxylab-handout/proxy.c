//
// Created by Bugen Zhao on 2020/3/7.
//

#define DBG

#include <stdio.h>
#include "csapp.h"
#include "proxy.h"
#include "rio_p.h"

#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main(int argc, char **argv) {
    int listenfd, connfd;
    char port[PORTLEN];
    string host, serv;
    struct sockaddr_storage clientaddr;
    socklen_t clientlen;
    int i;
    pthread_t tid;

    Signal(SIGINT, bye);
    // ignore broken pipe error caused by a prematurely closed connection
    Signal(SIGPIPE, SIG_IGN);

    if (argc >= 2)
        strcpy(port, argv[1]);
    else
        strcpy(port, "10188");

    listenfd = Open_listenfd(port);
    printf("%s is listening on port %s...\n\n", proxy_name, port);
    fflush(stdout);

    sbuf_init(&sbuf, SBUFSIZE);
    for (i = 0; i < NTHREADS; ++i) {
        Pthread_create(&tid, NULL, thread, NULL);
    }

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, host, MAXLINE, serv, MAXLINE, 0);
        sbuf_insert(&sbuf, connfd);
        printf("=> Accept connection %d from %s:%s\n", connfd, host, serv);
    }
}

void *thread(void *vargp) {
    int connfd;

    Pthread_detach(pthread_self());
    while (1) {
        connfd = sbuf_remove(&sbuf);
        process(connfd);
        Close(connfd);
        printf("=> Close connection %d\n\n\n", connfd);
    }
}

void process(int connfd) {
    string buf;
    string method, uri, version;
    rio_t rio;
    reqline_t reqline;
    reqheader_t reqheaders[32];
    size_t headers_cnt;
    int serverfd;

    Rio_readinitb(&rio, connfd);
    memset(method, 0, sizeof(string));
    memset(uri, 0, sizeof(string));
    memset(version, 0, sizeof(string));

    // get HTTP header / request line
    if (Rio_readlinebp(&rio, buf, MAXLINE) <= 0) return;

    printf("=> Source request:\n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);

    if (strcasecmp(method, "GET") != 0) {
        client_error(connfd, method, "501", "Not Implemented", "Not Implemented");
        return;
    }

    if (parse_uri(uri, &reqline) < 0) {
        client_error(connfd, uri, "501", "Not Implemented", "Protocol Not Implemented");
        return;
    }
    printf("[host: %s, port: %s, path: %s]\n", reqline.host, reqline.port, reqline.path);

    headers_cnt = parse_req_headers(&rio, &reqline, reqheaders);
    if (headers_cnt == 0) return;

    serverfd = forward(&reqline, reqheaders, &rio, headers_cnt);
    if (serverfd > 0) reply(serverfd, connfd);
}

size_t parse_req_headers(rio_t *rp, reqline_t *reqline, reqheader_t *reqheaders) {
    string buf;
    char *ptr;
    size_t idx = 5;
    int flag = 0;
    static const char *reserved[] = {"Host", "User-Agent", "Connection", "Proxy-Connection"};

    strcpy(reqheaders[0].key, "Host");
    sprintf(reqheaders[0].value, "%s:%s\r\n", reqline->host, reqline->port);
    strcpy(reqheaders[1].key, "User-Agent");
    strcpy(reqheaders[1].value, user_agent_val);
    strcpy(reqheaders[2].key, "Connection");
    strcpy(reqheaders[2].value, "close\r\n");
    strcpy(reqheaders[3].key, "Proxy-Connection");
    strcpy(reqheaders[3].value, "close\r\n");

    strcpy(reqheaders[4].key, "BProxy");
    strcpy(reqheaders[4].value, "(C) Bugen Zhao 2020\r\n");

    while (1) {
        if (Rio_readlinebp(rp, buf, MAXLINE) < 0) return 0;
        printf("%s", buf);

        if (strcmp(buf, "\r\n") == 0) {
            break;
        }
        ptr = strstr(buf, ": ");
        if (ptr == NULL) {
            fprintf(stderr, "Invalid header: %s", buf);
            exit(-1);
        }
        *ptr = '\0';

        if (strncasecmp(buf, reserved[0], strlen(reserved[0])) == 0) {
            strcpy(reqheaders[0].value, ptr + 2);
            continue;
        }

        for (int i = 1; i <= 3; ++i) {
            if (strncasecmp(buf, reserved[i], strlen(reserved[i])) == 0) {
                flag = 1;
                break;
            }
        }
        if (flag) {
            flag = 0;
            continue;
        }
        strcpy(reqheaders[idx].key, buf);
        strcpy(reqheaders[idx].value, ptr + 2);
        idx++;
    }

    return idx;
}

int parse_uri(string uri, reqline_t *reqline) {
    string buf;
    char *ptr = buf;
    char *port_p;
    char *slash_p;
    int offset = 7;

    strcpy(buf, uri);

    // strcasestr() acts incorrectly on Linux
    if (strstr(ptr, "http://") != ptr) {
        fprintf(stderr, "Not supported protocol: %s\n", uri);
        return -1;
    }

    ptr += offset;
    port_p = strstr(ptr, ":");
    slash_p = strstr(ptr, "/");
    if (slash_p == NULL) {
        fprintf(stderr, "Invalid uri: %s", uri);
        return -1;
    }

    if (port_p != NULL && port_p < slash_p) {
        *port_p = '\0';
        *slash_p = '\0';
        strcpy(reqline->host, ptr);
        strcpy(reqline->port, port_p + 1);
    } else {
        *slash_p = '\0';
        strcpy(reqline->host, ptr);
        strcpy(reqline->port, "80");
    }
    *slash_p = '/';
    strcpy(reqline->path, slash_p);

    return 0;
}

void client_error(int connfd, string cause, string errnum, string msg, string disc) {
    string html, buf;
    ull len;

    printf("Client error %s\n", errnum);

    sprintf(html, "<h1>%s %s</h1><p>%s: %s</p><em>%s</em>", errnum, msg, disc, cause, proxy_name);
    len = strlen(html);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, msg);
    sprintf(buf, "%sServer: %s\r\n", buf, proxy_name);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %llu\r\n", buf, len);
    sprintf(buf, "%sContent-type: text/html\r\n", buf);
    sprintf(buf, "%s\r\n", buf); // IMPORTANT!
    Rio_writenp(connfd, buf, strlen(buf));
    printf("%s", buf);

    Rio_writenp(connfd, html, len);
}

int forward(reqline_t *reqline, reqheader_t *reqheaders, rio_t *contentp, size_t headers_cnt) {
    int fd;
    int i;
    string buf;

    sprintf(buf, "GET %s HTTP/1.0\r\n", reqline->path);
    for (i = 0; i < headers_cnt; ++i)
        sprintf(buf, "%s%s: %s", buf, reqheaders[i].key, reqheaders[i].value);
    strcat(buf, "\r\n");

    // TODO: support POST
//    Rio_readnb(contentp, buf + strlen(buf), MAXLINE);

    printf("=> Forward request:\n%s", buf);

    fd = Open_clientfdp(reqline->host, reqline->port);
    if (fd < 0) return 0;
    Rio_writenp(fd, buf, MAXLINE);
    return fd;
}

void reply(int serverfd, int clientfd) {
    rio_t rio;
    char buf[MAXLINE];
    int n = 0;
    int total = 0;

    Rio_readinitb(&rio, serverfd);
    while ((n = Rio_readlinebp(&rio, buf, MAXLINE))) {
        // Connection reset
        if (n < 0) return;
        Rio_writenp(clientfd, buf, n);
        total += n;
    }

    printf("=> Replied %d bytes\n", total);
}