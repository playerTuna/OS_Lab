#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 200

int pipefd1[2], pipefd2[2]; // Pipe 1: Parent -> Child, Pipe 2: Child -> Parent

void INIT(void) {
    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}

void WRITE_TO_PIPE(int fd, const char *message) {
    write(fd, message, strlen(message) + 1);
}

void READ_FROM_PIPE(int fd, char *buffer) {
    read(fd, buffer, BUFFER_SIZE);
}

int main() {
    INIT();
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child Process
        close(pipefd1[1]); // Đóng đầu ghi của pipe 1 (chỉ đọc)
        close(pipefd2[0]); // Đóng đầu đọc của pipe 2 (chỉ ghi)

        char message[BUFFER_SIZE];
        while (1) {
            READ_FROM_PIPE(pipefd1[0], message);
            if (strcmp(message, "EXIT") == 0) break;
            printf("Child received: %s\n", message);

            printf("Child: ");
            fgets(message, BUFFER_SIZE, stdin);
            message[strcspn(message, "\n")] = 0; // Loại bỏ '\n'
            WRITE_TO_PIPE(pipefd2[1], message);
            if (strcmp(message, "EXIT") == 0) break;
        }
        
        close(pipefd1[0]);
        close(pipefd2[1]);
    } else { // Parent Process
        close(pipefd1[0]); // Đóng đầu đọc của pipe 1 (chỉ ghi)
        close(pipefd2[1]); // Đóng đầu ghi của pipe 2 (chỉ đọc)

        char message[BUFFER_SIZE];
        while (1) {
            printf("Parent: ");
            fgets(message, BUFFER_SIZE, stdin);
            message[strcspn(message, "\n")] = 0;
            WRITE_TO_PIPE(pipefd1[1], message);
            if (strcmp(message, "EXIT") == 0) break;

            READ_FROM_PIPE(pipefd2[0], message);
            if (strcmp(message, "EXIT") == 0) break;
            printf("Parent received: %s\n", message);
        }

        close(pipefd1[1]);
        close(pipefd2[0]);
    }

    return 0;
}
