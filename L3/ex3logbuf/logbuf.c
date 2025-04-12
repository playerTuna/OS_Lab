#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char** logbuf;
int count = 0;
int flushing = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct _args {
   unsigned int interval;
};

int wrlog(char** logbuf, char* newdata) {
   pthread_mutex_lock(&mutex);
   while (count >= MAX_BUFFER_SLOT || flushing) {
      pthread_cond_wait(&cond, &mutex);
   }
   logbuf[count] = strdup(newdata);
   count++;
   pthread_mutex_unlock(&mutex);
   return 0;
}

int flushlog(char** logbuf) {
   pthread_mutex_lock(&mutex);
   flushing = 1;
   for (int i = 0; i < count; i++) {
      printf("Slot %d: %s\n", i, logbuf[i]);
      free(logbuf[i]);
      logbuf[i] = NULL;
   }
   count = 0;
   flushing = 0;
   pthread_cond_broadcast(&cond);
   pthread_mutex_unlock(&mutex);
   return 0;
}

void* timer_start(void* args) {
   struct _args* targs = (struct _args*)args;
   while (1) {
      usleep(targs->interval);
      flushlog(logbuf);
   }
   return NULL;
}

void* writer_thread(void* arg) {
   int id = *(int*)arg;
   char str[MAX_LOG_LENGTH];
   snprintf(str, MAX_LOG_LENGTH, "%d", id);
   wrlog(logbuf, str);
   return NULL;
}

int main() {
   logbuf = (char**)malloc(sizeof(char*) * MAX_BUFFER_SLOT);

   pthread_t tid[MAX_LOOPS];
   pthread_t flusher;
   int ids[MAX_LOOPS];

   struct _args args;
   args.interval = 500000; // 500ms

   pthread_create(&flusher, NULL, timer_start, &args);

   for (int i = 0; i < MAX_LOOPS; i++) {
      ids[i] = i;
      pthread_create(&tid[i], NULL, writer_thread, &ids[i]);
   }

   for (int i = 0; i < MAX_LOOPS; i++) {
      pthread_join(tid[i], NULL);
   }

   sleep(3); // Allow final flush

   pthread_cancel(flusher); // Stop the flusher thread
   pthread_join(flusher, NULL);

   free(logbuf);
   return 0;
}
