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

// Cấu trúc lưu thông tin đánh giá phim
struct MovieRating {
    int movieId;
    int ratingCount;
    double ratingSum;
    double avgRating;
};

// Hàm đọc dữ liệu từ file và tính toán đánh giá phim
void processFile(const char* filename, struct MovieRating* movies) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    
    int userId, movieId, rating, timestamp;
    while (fscanf(file, "%d\t%d\t%d\t%d", &userId, &movieId, &rating, &timestamp) == 4) {
        // Skip lines that don't have required format
        if (movieId <= 0 || movieId >= MAX_MOVIES) {
            continue;
        }
        
        // Cập nhật thông tin đánh giá cho bộ phim
        movies[movieId].movieId = movieId;
        movies[movieId].ratingCount++;
        movies[movieId].ratingSum += rating;
    }
    
    fclose(file);
}

int main() {
    // Chuyển hướng stdout sang file output.txt
    FILE* output = freopen("output.txt", "w", stdout);
    
    // Tạo shared memory segment
    key_t key = ftok("problem1.c", 'R');
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory segment
    struct MovieRating* sharedMovies = (struct MovieRating*)shmat(shmid, NULL, 0);
    if (sharedMovies == (struct MovieRating*)-1) {
        perror("shmat");
        exit(1);
    }
    
    // Khởi tạo giá trị ban đầu cho shared memory
    memset(sharedMovies, 0, SHM_SIZE);
    
    // Fork tiến trình con thứ nhất
    pid_t pid1 = fork();
    
    if (pid1 < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid1 == 0) {
        // Tiến trình con 1
        printf("Child 1 (PID=%d): Processing movie-100k_1.txt\n", getpid());
        processFile("movie-100k_1.txt", sharedMovies);
        exit(0);
    }
    
    // Fork tiến trình con thứ hai
    pid_t pid2 = fork();
    
    if (pid2 < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid2 == 0) {
        // Tiến trình con 2
        printf("Child 2 (PID=%d): Processing movie-100k_2.txt\n", getpid());
        processFile("movie-100k_2.txt", sharedMovies);
        exit(0);
    }
    
    // Tiến trình cha chờ hai tiến trình con hoàn thành
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    // Tính điểm trung bình cho tất cả các phim
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
    
    // Detach và xóa shared memory segment
    shmdt(sharedMovies);
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}