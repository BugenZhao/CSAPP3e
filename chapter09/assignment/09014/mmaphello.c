//
// Created by Bugen Zhao on 2020/2/26.
//

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define DEF_MODE  S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
#define DEF_UMASK S_IWGRP|S_IWOTH

static const char *hello_file = "hello.txt";

void touch(char *filename, char *text) {
    umask(DEF_UMASK);
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, DEF_MODE);
    write(fd, text, strlen(text));
    close(fd);
}

void test(char *filename, char *text) {
    int fd = open(filename, O_RDONLY, 0);
    char buf[20];
    read(fd, buf, strlen(text));
    assert(strcmp(buf, text) == 0);
}

int main(int argc, char **argv) {
    touch(hello_file, "Hello, Bugen!");
    test(hello_file, "Hello, Bugen!");
    int fd = open(hello_file, O_RDWR, 0);   // open file and get file descriptor
    struct stat file_stat;
    fstat(fd, &file_stat);              // get file information
    char *buf = mmap(NULL, file_stat.st_size, PROT_WRITE, MAP_SHARED, fd, 0);   // map file to virtual memory
    *buf = 'J';
    munmap(buf, file_stat.st_size);
    test(hello_file, "Jello, Bugen!");
    exit(0);
}