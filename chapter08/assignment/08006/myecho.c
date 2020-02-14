#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
    puts("Command-line arguments:");
    for (int i = 0; i < argc; ++i) {
        printf("\targv[%2d]: %s\n", i, argv[i]);
    }
    puts("Environment variables:");
    for (int j = 0;; ++j) {
        if (envp[j] == NULL) break;
        printf("\tenvp[%2d]: %s\n", j, envp[j]);
    }

    return 0;
}
