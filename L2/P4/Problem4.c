#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>           
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define FILEPATH "shared.txt"
#define SIZE 4096

int main() {
    int fd;
    char *map;

    fd = open(FILEPATH, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    if (ftruncate(fd, SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    map = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    int pid = fork();

    if (pid == 0) {
        const char *message = "Hey dad! I am your child process.";
        memcpy(map, message, strlen(message));
        printf("Child: wrote message to memory-mapped file.\n");
    } else {
        wait(NULL);
        printf("Parent: read from shared memory -> %s\n", map);
    }

    munmap(map, SIZE);
    close(fd);
    unlink(FILEPATH); 

    return 0;
}
