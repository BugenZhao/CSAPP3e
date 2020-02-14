//
// Created by Bugen Zhao on 2020/2/14.
//
// NOT SAFE

#include <setjmp.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

sigjmp_buf buf;

void handler(int sig) {
    siglongjmp(buf, 1); // soft restart
}

int main() {
    if (!sigsetjmp(buf, 1)) { // 1 means save mask
        // first time
        signal(SIGINT, handler);
        puts("Starting");
        puts("Control-C to save me");
    } else {
        // from siglongjmp
        puts("\nRestarting");
    }

    int times = 30;
    while (times) {
        printf("%2d secs left...\n", times);
        times--;
        sleep(1);
    }
    puts("Bye");
    kill(0, SIGKILL);
    puts("OH NO");
}