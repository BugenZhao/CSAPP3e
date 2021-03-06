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
        // pre-processing
        Pthread_create(&tid, NULL, thread, NULL);
    }
    cache_init();

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
        printf("=> Close connection %d\n\n", connfd);
    }
}

void process(int connfd) {
    string buf;
    string method, uri, version;
    rio_t rio;
    reqline_t reqline;
    reqheader_t reqheaders[32];
    size_t headers_cnt = 0;
    size_t content_len = 0;
    cache_block_t *cache_block;
    int serverfd;
    int allow_cache = 0;

    Rio_readinitb(&rio, connfd);

    // get HTTP header / request line
    if (Rio_readlinebp(&rio, buf, MAXLINE) <= 0) return;

    printf("=> Source request:\n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);

    strcpy(reqline.method, method);
    if (strcasecmp(method, "GET") != 0 && strcasecmp(method, "POST") != 0 && strcasecmp(method, "HEAD") != 0) {
        client_error(connfd, method, "501", "Not Implemented", "Not Implemented");
        return;
    }

    // only cache for GET method
    allow_cache = strcasecmp(method, "GET") == 0;
    if (allow_cache) {
        // if cache exists, writing will be blocked between read_pre() and read_post()
        cache_block = cache_read_pre(uri);
        if (cache_block != NULL) {
            Rio_writenp(connfd, cache_block->cache_obj, cache_block->len);
            printf("=> Replied %zd bytes from cache\n", cache_block->len);
            cache_read_post(uri);
            return;
        }
    }

    // get host, port and path
    if (parse_uri(uri, &reqline) < 0) {
        client_error(connfd, uri, "501", "Not Implemented", "Protocol Not Implemented");
        return;
    }
    printf("[host: %s, port: %s, path: %s]\n", reqline.host, reqline.port, reqline.path);

    // parse headers and replace some
    headers_cnt = parse_req_headers(&rio, &reqline, reqheaders, &content_len);
    if (headers_cnt == 0) return;

    // forward to server and then reply to the client
    serverfd = forward(&reqline, reqheaders, headers_cnt, &rio, content_len);
    if (serverfd > 0) {
        reply(serverfd, connfd, uri, allow_cache);
        Close(serverfd);
        printf("=> Close connection %d\n\n", serverfd);
    }
}

size_t parse_req_headers(rio_t *rp, reqline_t *reqline, reqheader_t *reqheaders, size_t *content_len) {
    string buf;
    char *ptr;
    size_t idx = 5;
    int flag = 0;
    static const char *reserved[] = {"Host", "User-Agent", "Connection", "Proxy-Connection"};

    // reserved headers
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

        // if client attaches host, keep it
        if (strncasecmp(buf, reserved[0], strlen(reserved[0])) == 0) {
            strcpy(reqheaders[0].value, ptr + 2);
            continue;
        }

        // ignore other reserved headers
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

        // get content length for POST method
        if (strncasecmp(reqheaders[idx].key, "Content-Length", 14) == 0) {
            if (content_len) *content_len = atoi(reqheaders[idx].value);
        }
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

    // strcasestr() acts incorrectly on Linux?
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

    // the colon must precede the first slash
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

    printf("=> Client error %s\n", errnum);

    sprintf(html, "<h1>%s %s</h1><p>%s: %s</p><em>%s</em>", errnum, msg, disc, cause, proxy_name);
    len = strlen(html);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, msg);
    sprintf(buf, "%sServer: %s\r\n", buf, proxy_name);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %llu\r\n", buf, len);
    sprintf(buf, "%sContent-type: text/html\r\n", buf);
    sprintf(buf, "%s\r\n", buf); // IMPORTANT!
    Rio_writenp(connfd, buf, strlen(buf));

    Rio_writenp(connfd, html, len);
}

int forward(reqline_t *reqline, reqheader_t *reqheaders, size_t headers_cnt, rio_t *contentp, size_t content_len) {
    int fd;
    int i;
    string buf;

    // forward version 1.0
    sprintf(buf, "%s %s HTTP/1.0\r\n", reqline->method, reqline->path);
    for (i = 0; i < headers_cnt; ++i)
        sprintf(buf, "%s%s: %s", buf, reqheaders[i].key, reqheaders[i].value);
    strcat(buf, "\r\n");

    printf("=> Forward request:\n%s", buf);

    fd = Open_clientfdp(reqline->host, reqline->port);
    if (fd < 0) return 0;
    Rio_writenp(fd, buf, strlen(buf));

    // for POST
    if (content_len > 0) {
        printf("<Contents> (%zd bytes)\n\n", content_len);
        Rio_readnb(contentp, buf, content_len);
        Rio_writenp(fd, buf, content_len);
    }

    return fd;
}

void reply(int serverfd, int clientfd, string uri, int allow_cache) {
    rio_t rio;
    char buf[MAXLINE];
    char obj[MAX_OBJECT_SIZE];
    int n = 0;
    int total = 0;

    Rio_readinitb(&rio, serverfd);
    while ((n = Rio_readnb(&rio, buf, MAXLINE))) {
        // Connection reset
        if (n < 0) return;
        Rio_writenp(clientfd, buf, n);
        // CRITICAL: use memcpy() to avoid annoying '\0' problem!
        if (total + n < MAX_OBJECT_SIZE) memcpy(obj + total, buf, n);
        total += n;
    }

    // cache only if length and method meet the requirements
    if (allow_cache && total < MAX_OBJECT_SIZE) {
        cache_insert(uri, obj, total);
        printf("=> Cached %d bytes\n", total);
    }

    printf("=> Replied %d bytes\n", total);
}

/*
 * Caching policy: LRU
 * For the concurrency policy below, refer to Readers-writers Problem 1 (readers weakly first)
 */

void cache_init() {
    int i;
    string buf;

    cache.status = 0;
    cache.timestamp = 0;

    // use named semaphores
    cache.glb_mutex = Sem_open_and_unlink("/_brpoxy_glb_mutex", 1);
    for (i = 0; i < CACHE_OBJS_CNT; ++i) {
        cache.blocks[i].lru = 0;
        cache.blocks[i].reader_cnt = 0;
        cache.blocks[i].len = 0;
        sprintf(buf, "/_bproxy_wmutex_%d", i);
        cache.blocks[i].wmutex = Sem_open_and_unlink(buf, 1);
        sprintf(buf, "/_bproxy_cnt_mutex_%d", i);
        cache.blocks[i].cnt_mutex = Sem_open_and_unlink(buf, 1);
        memset(cache.blocks[i].cache_uri, 0, MAXLINE);
        memset(cache.blocks[i].cache_obj, 0, MAX_OBJECT_SIZE);
    }
}

cache_block_t *cache_read_pre(string uri) {
    int i;

    // TODO: maybe not so accurate
    P(cache.glb_mutex);
    cache.timestamp++;
    V(cache.glb_mutex);

    for (i = 0; i < CACHE_OBJS_CNT; ++i) {
        if (!cache_empty(i) && strcmp(uri, cache.blocks[i].cache_uri) == 0) {
            P(cache.blocks[i].cnt_mutex);
            cache.blocks[i].lru = cache.timestamp; // update lru timestamp
            cache.blocks[i].reader_cnt += 1;
            if (cache.blocks[i].reader_cnt == 1)
                P(cache.blocks[i].wmutex); // block writing
            V(cache.blocks[i].cnt_mutex);
            return cache.blocks + i;
        }
    }
    return NULL;
}

void cache_read_post(string uri) {
    int i;
    for (i = 0; i < CACHE_OBJS_CNT; ++i) {
        if (!cache_empty(i) && strcmp(uri, cache.blocks[i].cache_uri) == 0) {
            P(cache.blocks[i].cnt_mutex);
            cache.blocks[i].reader_cnt -= 1;
            if (cache.blocks[i].reader_cnt == 0)
                V(cache.blocks[i].wmutex); // unblock writing
            V(cache.blocks[i].cnt_mutex);
            return;
        }
    }
    unix_error("Cache error");
}

void cache_insert(string uri, char *data, size_t n) {
    int i, j;
    ull lru = -1;
    if (n > MAX_OBJECT_SIZE) return;

    // update if it already existed
    for (i = 0; i < CACHE_OBJS_CNT; ++i) {
        if (!cache_empty(i) && strcmp(uri, cache.blocks[i].cache_uri) == 0)
            break;
    }
    // not existed
    if (i == CACHE_OBJS_CNT) {
        i = 0;
        if (cache_full()) {
            // require evicting
            for (j = 0; j < CACHE_OBJS_CNT; ++j)
                if (cache.blocks[j].lru < lru) {
                    lru = cache.blocks[j].lru;
                    i = j;
                }
        } else {
            // find an empty slot
            while (!cache_empty(i)) i++;
        }
    }

    P(cache.blocks[i].wmutex);
    P(cache.glb_mutex);
    cache_set(i);
    V(cache.glb_mutex);

    // CRITICAL: use memcpy() to avoid annoying '\0' problem!
    memset(cache.blocks[i].cache_uri, 0, MAXLINE);
    strcpy(cache.blocks[i].cache_uri, uri);
    memset(cache.blocks[i].cache_obj, 0, MAX_OBJECT_SIZE);
    memcpy(cache.blocks[i].cache_obj, data, n);

    cache.blocks[i].len = n;
    cache.blocks[i].lru = cache.timestamp;
    V(cache.blocks[i].wmutex);
}