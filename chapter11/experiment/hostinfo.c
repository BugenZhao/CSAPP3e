//
// Created by Bugen Zhao on 2020/2/28.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    char domain[0x100];
    struct addrinfo *p, *listp, hints;
    int rc, flags;
    char buf_host[80];
    char buf_service[80];

    if (argc >= 2) strcpy(domain, argv[1]);
    else scanf("%s", domain);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rc = getaddrinfo(domain, NULL, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }

    flags = NI_NUMERICHOST;
    for (p = listp; p; p = p->ai_next) {
        getnameinfo(p->ai_addr, p->ai_addrlen, buf_host, 80, buf_service, 80, flags);
        printf("%s %s\n", buf_host, buf_service);
    }

    freeaddrinfo(listp);

    exit(0);
}