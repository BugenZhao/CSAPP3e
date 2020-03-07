//
// Created by Bugen Zhao on 2020/3/7.
//

#ifndef BUGENCSAPP3E_PROXY_H
#define BUGENCSAPP3E_PROXY_H

#include "sbuf.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *user_agent_val = "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

#define PORTLEN 6
#define KEYLEN 64

#define STATIC 1
#define DYNAMIC 0

#define GET 0
#define POST 1
#define HEAD 2

#define NTHREADS 8
#define SBUFSIZE (2*NTHREADS)


typedef char string[MAXLINE];
typedef unsigned long long ull;

typedef struct {
    string host;
    char port[PORTLEN];
    string path;
} reqline_t;

typedef struct {
    char key[KEYLEN];
    string value;
} reqheader_t;

const char proxy_name[] = "BProxy";
const char *methods[] = {"GET", "POST", "HEAD"};

sbuf_t sbuf;

void bye(int sig) {
    printf("Bye\n");
    exit(0);
}

void *thread(void *vargp);

void process(int connfd);

size_t parse_req_headers(rio_t *rp, reqline_t *reqline, reqheader_t *reqheaders);

int parse_uri(string uri, reqline_t *reqline);

void client_error(int connfd, string cause, string errnum, string msg, string disc);

int forward(reqline_t *reqline, reqheader_t *reqheaders, rio_t *contentp, size_t headers_cnt);

void reply(int serverfd, int clientfd);

#endif //BUGENCSAPP3E_PROXY_H
