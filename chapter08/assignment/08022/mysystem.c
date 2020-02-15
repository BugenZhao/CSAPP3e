//
// Created by Bugen Zhao on 2020/2/15.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int mysystem(const char *command) {
    if (command == NULL) return 1;
    pid_t pid;
    int status;

    if ((pid = fork()) == 0) {
        execl("/bin/sh", "sh", "-c", command, NULL);
    }
    if (waitpid(pid, &status, 0) == pid) {
        if (WIFEXITED(status)) return WEXITSTATUS(status);
        if (WIFSIGNALED(status)) return WTERMSIG(status);
        return status;
    } else {
        fprintf(stderr, "Failed to reap shell.\n");
        return -1;
    }
}

int main() {
    assert(mysystem("") == system(""));
    assert(mysystem("exit 8") == 8);
    assert(mysystem("ls") == 0);
}