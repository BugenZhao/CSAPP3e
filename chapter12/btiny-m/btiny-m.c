//
// Created by Bugen Zhao on 2020/3/7.
//

/*
 * A pre-threaded version of BTiny Web Server
 */

// TODO: dynamically increase or decrease the number of threads

#include "csapp.h"
#include "sbuf.h"

#pragma clang diagnostic ignored "-Wmissing-noreturn"

#define STATIC 1
#define DYNAMIC 0

#define GET 0
#define POST 1
#define HEAD 2

#define NTHREADS 8
#define SBUFSIZE (2*NTHREADS)

typedef char string[MAXLINE];
typedef unsigned long long ull;

const char server_name[] = "BTiny-m Web Server";
const char *methods[] = {"GET", "POST", "HEAD"};

sbuf_t sbuf;

extern void Rio_writenp(int fd, void *usrbuf, size_t n);

void bye(int sig) {
    printf("Bye\n");
    exit(0);
}

void sigchld_handler(int sig) {
    int old_errno = errno;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0);
    errno = old_errno;
}

void *thread(void *vargp);

void process(int connfd);

void skip_req_headers(rio_t *rp);

size_t read_req_headers(rio_t *rp);

int parse_uri(string uri, string filename, string cgiargs);

void serve_static(int connfd, string filename, int method);

void serve_dynamic(int connfd, string filename, string cgiargs, int method);

void client_error(int connfd, string cause, string errnum, string msg, string disc);

void get_filetype(string filename, string filetype);

int main(int argc, char **argv) {
    int listenfd, connfd;
    char port[6];
    string host, serv;
    struct sockaddr_storage clientaddr;
    socklen_t clientlen;
    string open_cmd = "open http://localhost:";
    int i;
    pthread_t tid;

    Signal(SIGINT, bye);
    Signal(SIGCHLD, sigchld_handler);
    // ignore broken pipe error caused by a prematurely closed connection
    Signal(SIGPIPE, SIG_IGN);

    if (argc >= 2)
        strcpy(port, argv[1]);
    else
        strcpy(port, "1018");

    listenfd = Open_listenfd(port);
    printf("%s is listening on port %s...\n\n", server_name, port);
    fflush(stdout);
    strcat(open_cmd, port);

#ifdef __APPLE__
    system(open_cmd);
#endif

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
//        process(connfd);
//        Close(connfd); // only the main process close the socket file
//        printf("Close connection from %s:%s\n\n\n", host, serv);
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
    string filename, cgiargs;
    rio_t rio;
    int is_static;
    size_t length;

    Rio_readinitb(&rio, connfd);
    // get HTTP header / request line
    if (Rio_readlineb(&rio, buf, MAXLINE) == 0) return;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);

    if (strcasecmp(method, "GET") == 0) {
        skip_req_headers(&rio);
        // serve GET request
        is_static = parse_uri(uri, filename, cgiargs);
        if (is_static) serve_static(connfd, filename, GET);
        else serve_dynamic(connfd, filename, cgiargs, GET);
    } else if (strcasecmp(method, "POST") == 0) {
        length = read_req_headers(&rio);
        // serve POST request;
        is_static = parse_uri(uri, filename, cgiargs);
        if (is_static) serve_static(connfd, filename, POST);
        else {
            // read POST args
            Rio_readnb(&rio, cgiargs, length);
            serve_dynamic(connfd, filename, cgiargs, POST);
        }
    } else if (strcasecmp(method, "HEAD") == 0) {
        skip_req_headers(&rio);
        // serve HEAD request
        is_static = parse_uri(uri, filename, cgiargs);
        if (is_static) serve_static(connfd, filename, HEAD);
        else serve_dynamic(connfd, filename, cgiargs, HEAD);
    } else {
        client_error(connfd, method, "501", "Not Implemented", "Not Implemented");
    }
}

void skip_req_headers(rio_t *rp) {
    string buf;
    do {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    } while (strcmp(buf, "\r\n") != 0);
}

size_t read_req_headers(rio_t *rp) {
    string buf;
    size_t length = 0;

    // get post content length from the header
    do {
        Rio_readlineb(rp, buf, MAXLINE);
        if (strncasecmp(buf, "Content-length:", 15) == 0)
            sscanf(buf + 15, "%lu", &length);
        printf("%s", buf);
    } while (strcmp(buf, "\r\n") != 0);

    return length;
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

void serve_static(int connfd, string filename, int method) {
    struct stat sbuf;
    string filetype;
    string buf;
    int fd;
    char *filebuf;
    off_t filesize;

    printf("=> Static content\n");

    if (stat(filename, &sbuf) < 0) {
        client_error(connfd, filename, "404", "Not Found", "Not Found");
        return;
    }
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
        client_error(connfd, filename, "403", "Forbidden", "No permission to read");
        return;
    }
    filesize = sbuf.st_size;

    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: %s\r\n", buf, server_name);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %llu\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n", buf, filetype);
    sprintf(buf, "%s\r\n", buf); // IMPORTANT!
    Rio_writenp(connfd, buf, strlen(buf));
    printf("%s", buf);

    if (method == HEAD) return;

    fd = Open(filename, O_RDONLY, 0);
    filebuf = Malloc(filesize);
    Rio_readn(fd, filebuf, filesize);
//    filebuf = Mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    Close(fd);
    Rio_writenp(connfd, filebuf, filesize);
//    Munmap(filebuf, filesize);
    Free(filebuf);
}

void serve_dynamic(int connfd, string filename, string cgiargs, int method) {
    struct stat sbuf;
    string filetype;
    string buf;
    char **argv = {NULL};

    printf("=> Dynamic content\n");

    if (stat(filename, &sbuf) < 0) {
        client_error(connfd, filename, "404", "Not Found", "Not Found");
        return;
    }
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
        client_error(connfd, filename, "403", "Forbidden", "No permission to execute");
        return;
    }

    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: %s\r\n", buf, server_name);
    Rio_writenp(connfd, buf, strlen(buf));
    printf("%s", buf);

    if (Fork() == 0) {
        // restore default pipe error handler
        Signal(SIGPIPE, SIG_DFL);
        setenv("QUERY_STRING", cgiargs, 1);
        // TODO: fix: no response after setting request_method
//        setenv("REQUEST_METHOD", methods[method], 1);
//        printf("I'M HERE!!!!!!!!!!\n");
        Dup2(connfd, STDOUT_FILENO);
        Execve(filename, argv, environ);
    }
//    Wait(NULL);
}

void client_error(int connfd, string cause, string errnum, string msg, string disc) {
    string html, buf;
    ull len;

    printf("Client error %s\n", errnum);

    sprintf(html, "<h1>%s %s</h1><p>%s: %s</p><em>%s</em>", errnum, msg, disc, cause, server_name);
    len = strlen(html);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, msg);
    sprintf(buf, "%sServer: %s\r\n", buf, server_name);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %llu\r\n", buf, len);
    sprintf(buf, "%sContent-type: text/html\r\n", buf);
    sprintf(buf, "%s\r\n", buf); // IMPORTANT!
    Rio_writenp(connfd, buf, strlen(buf));
    printf("%s", buf);

    Rio_writenp(connfd, html, len);
}

void get_filetype(string filename, string filetype) {
    // get mime type by filename
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