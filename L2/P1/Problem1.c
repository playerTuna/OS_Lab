#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <string.h>

#define MAX_MOVIES 1682
#define FILE1 "./movie-100k-split/movie-100k_1.txt"
#define FILE2 "./movie-100k-split/movie-100k_2.txt"

// format input FILE1 and FILE2
// userID <tab> movieID <tab> rating <tab> timeStamp

// Shared memory structure
typedef struct
{
    int count[MAX_MOVIES];
    double sum[MAX_MOVIES];
} SharedData;

// Function to read a file and compute movie ratings
void process_file(const char *filename, SharedData *shared_data)
{
    // Open file
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        exit(1);
    }
    // Extract data from file
    int userID, movieID, rating, timestamp;
    while (fscanf(file, "%d\t%d\t%d\t%d", &userID, &movieID, &rating, &timestamp) != EOF)
    {
        shared_data->count[movieID]++;
        shared_data->sum[movieID] += rating;
    }
    fclose(file);
}

int main()
{
    int shmid;
    SharedData *shared_data;

    // Create shared memory
    shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget failed");
        exit(1);
    }

    // Attach shared memory
    shared_data = (SharedData *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    // Initialize shared memory
    memset(shared_data, 0, sizeof(SharedData));

    pid_t pid1, pid2;
    // Create first child process
    if ((pid1 = fork()) == 0)
    {
        process_file(FILE1, shared_data);
        shmdt(shared_data);
        exit(0);
    }

    // Create second child process
    if ((pid2 = fork()) == 0)
    {
        process_file(FILE2, shared_data);
        shmdt(shared_data);
        exit(0);
    }

    // Parent waits for both children to complete
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    // Compute and print average ratings
    printf("MovieID\tAverage Rating\n");
    for (int i = 1; i <= MAX_MOVIES; i++)
    {
        if (shared_data->count[i] > 0)
        {
            double avg = shared_data->sum[i] / shared_data->count[i];
            printf("%d\t%.2f\n", i, avg);
        }
    }

    // Detach and remove shared memory
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
/* Compile with: gcc -o Problem1 Problem1.c -lpthread
   Run with: ./Problem1
   Code by: Dang Pham Gia Long
*/