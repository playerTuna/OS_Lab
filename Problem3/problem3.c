#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY 1234
#define MAX_TEXT 512

typedef struct {
    long msg_type;
    char text[MAX_TEXT];
} message_buffer;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [sender|receiver]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int msgid = msgget(QUEUE_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }
    message_buffer msg;
    if (strcmp(argv[1], "sender") == 0) {
        msg.msg_type = 1;
        strcpy(msg.text, "Hello from sender!");
        if (msgsnd(msgid, &msg, strlen(msg.text) + 1, 0) == -1) {
            perror("msgsnd failed");
            exit(EXIT_FAILURE);
        }
        printf("[Sender] Message sent: \"%s\"\n", msg.text);
        if (msgrcv(msgid, &msg, MAX_TEXT, 2, 0) == -1) {
            perror("msgrcv failed");
            exit(EXIT_FAILURE);
        }
        printf("[Sender] Acknowledgment received: \"%s\"\n", msg.text);
    } else if (strcmp(argv[1], "receiver") == 0) {
        if (msgrcv(msgid, &msg, MAX_TEXT, 1, 0) == -1) {
            perror("msgrcv failed");
            exit(EXIT_FAILURE);
        }
        printf("[Receiver] Message received: \"%s\"\n", msg.text);
        msg.msg_type = 2;
        strcat(msg.text, " [Acknowledgment from receiver]");
        if (msgsnd(msgid, &msg, strlen(msg.text) + 1, 0) == -1) {
            perror("msgsnd failed");
            exit(EXIT_FAILURE);
        }
        printf("[Receiver] Acknowledgment sent.\n");
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("msgctl failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}