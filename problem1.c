#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>



float process (const char * filename){
    FILE * file = fopen(filename,"r");
    if (file == NULL){
        printf("Error opening file");
        return -1;
    }
    char line [256];
    int userID,movID,rating,total = 0,count = 0;
    long int timestamp = 0;
    while (!feof(file)){
        fgets(line,sizeof(line),file);
        sscanf(line, "%d\t%d\t%d\t%ld", &userID, &movID, &rating, &timestamp);
        total += rating;
        count++;
    }
    fclose(file);
    return (count==0)?0:(float)total/count;
}

int main() {
    key_t key = ftok("shmfile",65);
    float shmid = shmget(key,sizeof(int)*2,0666|IPC_CREAT);
    float * shm_data = (float*)shmat(shmid,NULL,0);
    pid_t pid1 = fork();
    if (pid1==0){
        shm_data[0] = process("movie-100k_1.txt");
        exit(0);
    }
    pid_t pid2 = fork();
    if (pid2==0){
        shm_data[1] = process("movie-100k_2.txt");
        exit(0);
    }
    wait(NULL);
    wait(NULL);

    printf("Average Rating from file 1: %.2f\n", shm_data[0] );
    printf("Average Rating from file 2: %.2f\n", shm_data[1] );

    shmdt(shm_data);
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}