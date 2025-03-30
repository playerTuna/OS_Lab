#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
#define MSG_KEY 0x123

struct my_msgbuf {
    long mtype;
    char mtext[200];
};

int main() {
    struct my_msgbuf buf;
    int msqid;
    key_t key = MSG_KEY;

    if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }

    printf("MSGRCV: Receive message from MSGSND and send reply to MSGSND.\n");

    while (1) {
        // Nhận tin nhắn từ MSGSND
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 1, 0) == -1) {
            perror("msgrcv");
        }

        printf("RECEIVED: %s\n", buf.mtext);

        if (strcmp(buf.mtext, "EXIT") == 0) {
            printf("MSGRCV: Exiting.\n");
            break;
        }

        // Gửi phản hồi lại MSGSND
        printf("SEND REPLY: ");
        fgets(buf.mtext, sizeof(buf.mtext), stdin);
        buf.mtext[strcspn(buf.mtext, "\n")] = '\0'; // Remove newline
        buf.mtype = 2;

        if (msgsnd(msqid, &buf, sizeof(buf.mtext), 0) == -1) {
            perror("msgsnd");
        }
    }

    return 0;
}
