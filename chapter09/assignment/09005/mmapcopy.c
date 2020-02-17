//
// Created by Bugen Zhao on 2020/2/17.
//

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) return -1;
    int fd = open(argv[1], O_RDONLY);   // open file and get file descriptor
    struct stat file_stat;
    fstat(fd, &file_stat);              // get file information
    char *buf = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);   // map file to virtual memory
    write(1, buf, file_stat.st_size);   // write to stdout
    exit(0);
}