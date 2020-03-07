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

void reply(int serverfd, int clientfd);

void cache_init() {
    int i;
    string buf;

    cache.status = 0;
    cache.timestamp = 0;
    cache.glb_mutex = Sem_open_and_unlink("/_brpoxy_glb_mutex", 1);
    for (i = 0; i < CACHE_OBJS_CNT; ++i) {
        cache.blocks[i].lru = 0;
        cache.blocks[i].reader_cnt = 0;
        sprintf(buf, "/_bproxy_wmutex_%d", i);
        cache.blocks[i].wmutex = Sem_open_and_unlink(buf, 1);
        sprintf(buf, "/_bproxy_cnt_mutex_%d", i);
        cache.blocks[i].cnt_mutex = Sem_open_and_unlink(buf, 1);
    }
}

inline int cache_full() {
    return cache.status == (1 << CACHE_OBJS_CNT) - 1;
}

inline int cache_empty(unsigned i) {
    return (cache.status & (1u << i)) == 0;
}

inline void cache_set(unsigned i) {
    cache.status |= (1u << i);
}

inline void cache_clr(unsigned i) {
    cache.status &= ~(1u << i);
}

char *cache_read_pre(string uri);

void cache_read_post(string uri);

void cache_insert(string uri, char *data, size_t n);

#endif //BUGENCSAPP3E_PROXY_H
