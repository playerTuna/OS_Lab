#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define PERMS 0644
#define MSG_KEY_1 0x111   // Queue để gửi
#define MSG_KEY_2 0x222   // Queue để nhận

struct msgbuf {
    long mtype;
    char mtext[200];
};

// Biến để xác định tiến trình là 1 hay 2
int is_proc1 = 1;

void* sender(void* arg) {
    int msgid = msgget(is_proc1 ? MSG_KEY_1 : MSG_KEY_2, PERMS | IPC_CREAT);
    if (msgid == -1) { perror("msgget-sender"); exit(1); }

    struct msgbuf msg;
    msg.mtype = 1;

    while (1) {
        fgets(msg.mtext, sizeof(msg.mtext), stdin);
        msg.mtext[strcspn(msg.mtext, "\n")] = '\0';

        if (strcmp(msg.mtext, "exit") == 0) {
            msgsnd(msgid, &msg, strlen(msg.mtext)+1, 0);
            break;
        }

        if (msgsnd(msgid, &msg, strlen(msg.mtext)+1, 0) == -1)
            perror("msgsnd");
    }

    return NULL;
}

void* receiver(void* arg) {
    int msgid = msgget(is_proc1 ? MSG_KEY_2 : MSG_KEY_1, PERMS | IPC_CREAT);
    if (msgid == -1) { perror("msgget-receiver"); exit(1); }

    struct msgbuf msg;

    while (1) {
        if (msgrcv(msgid, &msg, sizeof(msg.mtext), 0, 0) == -1)
            perror("msgrcv");
        else {
            if (strcmp(msg.mtext, "exit") == 0) break;
            printf("%s\n", msg.mtext);
        }
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2 || (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "2") != 0)) {
        printf("Cách dùng: %s [1|2]\n", argv[0]);
        exit(1);
    }

    is_proc1 = (strcmp(argv[1], "1") == 0);

    pthread_t t_send, t_recv;

    pthread_create(&t_send, NULL, sender, NULL);
    pthread_create(&t_recv, NULL, receiver, NULL);

    pthread_join(t_send, NULL);
    pthread_cancel(t_recv); // Kết thúc receiver khi sender thoát

    // Chỉ tiến trình 1 mới xóa message queue để tránh xung đột
    if (is_proc1) {
        msgctl(msgget(MSG_KEY_1, PERMS), IPC_RMID, NULL);
        msgctl(msgget(MSG_KEY_2, PERMS), IPC_RMID, NULL);
    }

    return 0;
}
