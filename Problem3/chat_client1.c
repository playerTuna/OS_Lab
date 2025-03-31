#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define PERMS 0644
#define MSG_KEY_1_TO_2 0x123  // Hàng đợi để gửi tin từ client1 đến client2
#define MSG_KEY_2_TO_1 0x124  // Hàng đợi để nhận tin từ client2

// Cấu trúc thông điệp
struct message {
    long mtype;
    char mtext[200];
};

// Biến toàn cục để điều khiển luồng
int should_exit = 0;

// Hàm thread nhận tin nhắn
void* receiver_thread(void* arg) {
    int msqid = *((int*)arg);
    struct message msg;

    printf("Receiver thread started. Waiting for messages...\n");
    printf("You: ");
    fflush(stdout);
    // Vòng lặp chính để nhận tin nhắn
    while (!should_exit) {
        // Nhận tin nhắn, không block nếu không có tin
        if (msgrcv(msqid, &msg, sizeof(msg.mtext), 0, IPC_NOWAIT) == -1) {
            if (errno != ENOMSG) {  // Chỉ hiển thị lỗi nếu không phải vì hàng đợi trống
                perror("msgrcv");
            }
        }
        else {
            printf("\nClient2: \"%s\"\n", msg.mtext);

            // Kiểm tra tin nhắn thoát
            if (strcmp(msg.mtext, "exit") == 0) {
                printf("Client2 has exited. Press Enter to exit too.\n");
                should_exit = 1;
                break;
            }
            printf("You: ");
            fflush(stdout);
        }
        usleep(500000);  // Đợi 0.5 giây để giảm tải CPU
    }

    pthread_exit(NULL);
}

int main() {
    struct message msg;
    int send_msqid, recv_msqid;
    int len;
    pthread_t tid;

    // Tạo hoặc kết nối tới message queue để gửi tin
    if ((send_msqid = msgget(MSG_KEY_1_TO_2, PERMS | IPC_CREAT)) == -1) {
        perror("msgget (send)");
        exit(1);
    }

    // Tạo hoặc kết nối tới message queue để nhận tin
    if ((recv_msqid = msgget(MSG_KEY_2_TO_1, PERMS | IPC_CREAT)) == -1) {
        perror("msgget (receive)");
        exit(1);
    }

    printf("=== Client 1 Chat Program ===\n");
    printf("Type messages to send, 'exit' to quit\n");

    // Khởi tạo thread nhận tin nhắn
    if (pthread_create(&tid, NULL, receiver_thread, &recv_msqid) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // Vòng lặp chính để gửi tin nhắn
    msg.mtype = 1;
    while (!should_exit) {
        printf("You: ");
        if (fgets(msg.mtext, sizeof(msg.mtext), stdin) == NULL) {
            break;
        }

        len = strlen(msg.mtext);
        if (msg.mtext[len - 1] == '\n') msg.mtext[len - 1] = '\0';

        // Gửi tin nhắn
        if (msgsnd(send_msqid, &msg, len, 0) == -1) {
            perror("msgsnd");
        }

        // Kiểm tra lệnh thoát
        if (strcmp(msg.mtext, "exit") == 0) {
            should_exit = 1;
            break;
        }
    }

    // Đợi thread nhận tin nhắn kết thúc
    pthread_join(tid, NULL);

    printf("Chat session ended.\n");

    return 0;
}