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

    printf("MSGSND: Send message to MSGRCV and receive reply from MSGRCV. Type 'EXIT' to quit.\n");

    while (1) {
        printf("SEND: ");
        fgets(buf.mtext, sizeof(buf.mtext), stdin);
        buf.mtext[strcspn(buf.mtext, "\n")] = '\0'; // Remove newline
        buf.mtype = 1;

        if (msgsnd(msqid, &buf, sizeof(buf.mtext), 0) == -1) {
            perror("msgsnd");
        }

        if (strcmp(buf.mtext, "EXIT") == 0) {
            printf("MSGSND: Exiting.\n");
            break;
        }

        // Nhận phản hồi từ MSGRCV
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 2, 0) == -1) {
            perror("msgrcv");
        } else {
            printf("RECEIVED REPLY: %s\n", buf.mtext);
        }
    }

    return 0;
}
