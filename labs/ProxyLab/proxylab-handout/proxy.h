//
// Created by Bugen Zhao on 2020/3/7.
//

#ifndef BUGENCSAPP3E_PROXY_H
#define BUGENCSAPP3E_PROXY_H

#include "sbuf.h"
#include "bzcsapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define CACHE_OBJS_CNT ((int)(MAX_CACHE_SIZE / MAX_OBJECT_SIZE))

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *user_agent_val = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_3) "
                                    "AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.5 Safari/605.1.15 "
                                    "BugenProxy\r\n";

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
typedef char cobj_t[MAX_OBJECT_SIZE];
typedef unsigned long long ull;

typedef struct {
    char method[10];
    string host;
    char port[PORTLEN];
    string path;
} reqline_t;

typedef struct {
    char key[KEYLEN];
    string value;
} reqheader_t;

typedef struct {
    cobj_t cache_obj;
    string cache_uri;
    size_t len;
    ull lru;
    int reader_cnt;
    sem_t *wmutex;
    sem_t *cnt_mutex;
} cache_block_t;

typedef struct {
    cache_block_t blocks[CACHE_OBJS_CNT];
    ull status;
    ull timestamp;
    sem_t *glb_mutex;
} cache_t;

const char proxy_name[] = "BProxy";
const char *methods[] = {"GET", "POST", "HEAD"};

sbuf_t sbuf;
cache_t cache;


void bye(int sig) {
    printf("Bye\n");
    exit(0);
}

void *thread(void *vargp);

void process(int connfd);

size_t parse_req_headers(rio_t *rp, reqline_t *reqline, reqheader_t *reqheaders, size_t *content_len);

int parse_uri(string uri, reqline_t *reqline);

void client_error(int connfd, string cause, string errnum, string msg, string disc);

int forward(reqline_t *reqline, reqheader_t *reqheaders, size_t headers_cnt, rio_t *contentp, size_t content_len);

void reply(int serverfd, int clientfd, string uri, int allow_cache);

void cache_init();

int cache_full() {
    return cache.status == (1 << CACHE_OBJS_CNT) - 1;
}

int cache_empty(unsigned i) {
    return (cache.status & (1u << i)) == 0;
}

void cache_set(unsigned i) {
    cache.status |= (1u << i);
}

void cache_clr(unsigned i) {
    cache.status &= ~(1u << i);
}

cache_block_t *cache_read_pre(string uri);

void cache_read_post(string uri);

void cache_insert(string uri, char *data, size_t n);

#endif //BUGENCSAPP3E_PROXY_H
