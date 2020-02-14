//
// Created by Bugen Zhao on 2020/2/14.
//

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

unsigned int snooze(unsigned int secs) {
    unsigned int rem = sleep(secs);
    printf("Slept for %d of %d secs.\n", secs - rem, secs);
    return rem;
}

void sigint_handler(int sig) {
    return;
}

int main(int argc, char **argv) {
    if (argc != 2) exit(-1);
    signal(SIGINT, sigint_handler);
    snooze(atoi(argv[1]));
    exit(0);
}