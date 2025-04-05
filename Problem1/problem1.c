#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAX_MOVIES 1683
#define SHM_SIZE (MAX_MOVIES * sizeof(struct MovieRating))

struct MovieRating {
    int movieId;
    int ratingCount;
    double ratingSum;
    double avgRating;
};

void processFile(const char* filename, struct MovieRating* movies) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    
    int userId, movieId, rating, timestamp;
    while (fscanf(file, "%d\t%d\t%d\t%d", &userId, &movieId, &rating, &timestamp) == 4) {
        if (movieId <= 0 || movieId >= MAX_MOVIES) {
            continue;
        }
        
        movies[movieId].movieId = movieId;
        movies[movieId].ratingCount++;
        movies[movieId].ratingSum += rating;
    }
    
    fclose(file);
}

int main() {
    FILE* output = freopen("output.txt", "w", stdout);
    
    key_t key = ftok("problem1.c", 'R');
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    struct MovieRating* sharedMovies = (struct MovieRating*)shmat(shmid, NULL, 0);
    if (sharedMovies == (struct MovieRating*)-1) {
        perror("shmat");
        exit(1);
    }
    
    memset(sharedMovies, 0, SHM_SIZE);
    
    pid_t pid1 = fork();
    
    if (pid1 < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid1 == 0) {
        printf("Child 1 (PID=%d): Processing movie-100k_1.txt\n", getpid());
        processFile("movie-100k_1.txt", sharedMovies);
        exit(0);
    }
    
    pid_t pid2 = fork();
    
    if (pid2 < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid2 == 0) {
        printf("Child 2 (PID=%d): Processing movie-100k_2.txt\n", getpid());
        processFile("movie-100k_2.txt", sharedMovies);
        exit(0);
    }
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    int validMovies = 0;
    double totalAvgRating = 0.0;
    
    for (int i = 1; i < MAX_MOVIES; i++) {
        if (sharedMovies[i].ratingCount > 0) {
            sharedMovies[i].avgRating = sharedMovies[i].ratingSum / sharedMovies[i].ratingCount;
            printf("MovieID: %d\t Rating Count: %d\t Average Rating: %.2f\n", sharedMovies[i].movieId, sharedMovies[i].ratingCount, sharedMovies[i].avgRating);
            validMovies++;
            totalAvgRating += sharedMovies[i].avgRating;
        }
    }
    
    printf("\nSummary: %d movies with average rating = %.2f\n", validMovies, totalAvgRating/validMovies);
    
    shmdt(sharedMovies);
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}