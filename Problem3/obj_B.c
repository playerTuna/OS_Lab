#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define KEY_RCV 0x123
#define KEY_SEND 0x456

struct msgbuf {
    long mtype;
    char mtext[200];
};

int qid_send, qid_recv;

void* sender(void* arg) {
    struct msgbuf msg;
    msg.mtype = 1;
    char input[200];
    while (1) {
        printf("You: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        strcpy(msg.mtext, input);
        msgsnd(qid_send, &msg, strlen(msg.mtext) + 1, 0);
        if (strcmp(input, "exit") == 0) break;
    }
    pthread_exit(NULL);
}

void* receiver(void* arg) {
    struct msgbuf msg;
    while (1) {
        msgrcv(qid_recv, &msg, sizeof(msg.mtext), 0, 0);
        if (strcmp(msg.mtext, "exit") == 0) break;
        printf("\n");
        printf("Friend: %s\n", msg.mtext);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t t_send, t_recv;

    qid_send = msgget(KEY_SEND, IPC_CREAT | 0644);
    qid_recv = msgget(KEY_RCV, IPC_CREAT | 0644);

    pthread_create(&t_send, NULL, sender, NULL);
    pthread_create(&t_recv, NULL, receiver, NULL);

    pthread_join(t_send, NULL);
    pthread_join(t_recv, NULL);

    msgctl(qid_send, IPC_RMID, NULL);
    msgctl(qid_recv, IPC_RMID, NULL);

    return 0;
}
