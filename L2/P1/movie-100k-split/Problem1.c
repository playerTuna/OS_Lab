#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#include <unistd.h>
#include <sys/wait.h>

#define MAX_MOVIES 1682
#define SHMKEY 0x1512

#include <string.h>

typedef struct {
    int Sum_Rating[MAX_MOVIES];
    int Count_Rating[MAX_MOVIES];
} buf;

int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    if (argc > 1 && strcmp((argv[1]), "CFO") == 0) fp = fopen("output.txt", "w");

    int shmid = shmget(SHMKEY, sizeof(buf), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        return 1;
    }

    buf *shared_buf = (buf *)shmat(shmid, NULL, 0);
    if (shared_buf == (buf *)-1) {
        perror("shmat failed");
        exit(1);
    }
    memset(shared_buf, 0, sizeof(buf));
    int *Sum_Rating = shared_buf->Sum_Rating;
    int *Count_Rating = shared_buf->Count_Rating;

    int pid = fork(); 

    char *fileName;

    if (pid == 0) { 
        fileName = "./movie-100k-split/movie-100k_1.txt";
    } else if (pid > 0) { 
        fileName = "./movie-100k-split/movie-100k_2.txt";
        wait(NULL); // Wait for the child process to finish
    } else { 
        perror("Fork failed");
        return -1;
    }

    FILE *file = fopen(fileName, "r"); 
    if (file == NULL) {
        printf("Cannot open file!\n");
        return 1;
    }

    char line[256];
    int user_id, movie_id, rating, timestamp;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d %d %d %d", &user_id, &movie_id, &rating, &timestamp) == 4) {
            if (movie_id >= 0 && movie_id < MAX_MOVIES) {
                Sum_Rating[movie_id] += rating;
                Count_Rating[movie_id]++;
            }
        }
    }


    if(pid > 0){
        for(int i = 0; i < MAX_MOVIES; i++){
            if(Count_Rating[i] != 0){
                if (fp) {
                    fprintf(fp, "Movie ID: %d, Average Rating: %.2f\n", i,  Sum_Rating[i] * 1.f / Count_Rating[i]);
                } else {
                    printf("Movie ID: %d, Average Rating: %.2f\n", i,  Sum_Rating[i] * 1.f / Count_Rating[i]);
                }
            }
        }

        if(fp)  fclose(fp);

        shmdt(shared_buf);
        shmctl(shmid, IPC_RMID, NULL);
    }
    else shmdt(shared_buf);

    fclose(file); 
    return 0;
}
