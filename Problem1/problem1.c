#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define MAX_MOVIES 2000
#define LINE_LENGTH 100

typedef struct {
    int movieID;
    float totalRating;
    int count;
} MovieRating;

typedef struct {
    MovieRating ratings1[MAX_MOVIES];
    MovieRating ratings2[MAX_MOVIES];
} SharedData;

int findMovieIndex(MovieRating* ratings, int movieID, int* numMovies) {
    for (int i = 0; i < *numMovies; i++) {
        if (ratings[i].movieID == movieID) return i;
    }
    ratings[*numMovies].movieID = movieID;
    ratings[*numMovies].totalRating = 0;
    ratings[*numMovies].count = 0;
    (*numMovies)++;
    return (*numMovies) - 1;
}

void processFile(const char* filename, MovieRating* ratings, int* numMovies) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("File open failed");
        exit(1);
    }

    char line[LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        int userID, movieID;
        float rating;
        long timestamp;
        sscanf(line, "%d\t%d\t%f\t%ld", &userID, &movieID, &rating, &timestamp);

        int idx = findMovieIndex(ratings, movieID, numMovies);
        ratings[idx].totalRating += rating;
        ratings[idx].count += 1;
    }

    fclose(fp);
}

int cmpMovieRating(const void* a, const void* b) {
    MovieRating* m1 = (MovieRating*) a;
    MovieRating* m2 = (MovieRating*) b;
    return m1->movieID - m2->movieID;
}

int main() {
    freopen("output.txt", "w", stdout); // Ghi kết quả ra file

    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
    SharedData* sharedData = (SharedData*) shmat(shmid, NULL, 0);

    int num1 = 0, num2 = 0;

    pid_t pid1 = fork();
    if (pid1 == 0) {
        processFile("movie-100k-split/movie-100k_1.txt", sharedData->ratings1, &num1);
        shmdt(sharedData);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        processFile("movie-100k-split/movie-100k_2.txt", sharedData->ratings2, &num2);
        shmdt(sharedData);
        exit(0);
    }

    wait(NULL);
    wait(NULL);

    MovieRating merged[MAX_MOVIES];
    int mergedCount = 0;

    for (int i = 0; i < MAX_MOVIES && sharedData->ratings1[i].count > 0; i++) {
        int idx = findMovieIndex(merged, sharedData->ratings1[i].movieID, &mergedCount);
        merged[idx].totalRating += sharedData->ratings1[i].totalRating;
        merged[idx].count += sharedData->ratings1[i].count;
    }

    for (int i = 0; i < MAX_MOVIES && sharedData->ratings2[i].count > 0; i++) {
        int idx = findMovieIndex(merged, sharedData->ratings2[i].movieID, &mergedCount);
        merged[idx].totalRating += sharedData->ratings2[i].totalRating;
        merged[idx].count += sharedData->ratings2[i].count;
    }

    qsort(merged, mergedCount, sizeof(MovieRating), cmpMovieRating);

    for (int i = 0; i < mergedCount; i++) {
        float avg = merged[i].totalRating / merged[i].count;
        printf("Movie ID: %d, Average Rating: %.2f\n", merged[i].movieID, avg);
    }
    shmdt(sharedData);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}