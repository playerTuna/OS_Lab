#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];

int count;
void flushlog();

pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

struct _args
{
   unsigned int interval;
};

void *wrlog(void *data)
{
   char str[MAX_LOG_LENGTH];
   int id = *(int*) data;

   usleep(20);
   sprintf(str, "%d", id);
   // Khóa mutex để truy cập vùng găng
   pthread_mutex_lock(&log_mutex);
   
   // Nếu buffer đã đầy, chờ đến khi buffer được flush
   while (count >= MAX_BUFFER_SLOT) {
       // printf("Buffer full: wrlog(%d) waiting...\n", id);
       pthread_cond_wait(&buffer_not_full, &log_mutex);
   }
   // Ghi log vào buffer
   strcpy(logbuf[count], str);
   count = (count > MAX_BUFFER_SLOT)? count :(count + 1); /* Only increase count to size MAX_BUFFER_SLOT*/
   printf("wrlog(): %d \n", id);
   // Báo hiệu buffer không rỗng
   pthread_cond_signal(&buffer_not_empty);
   pthread_mutex_unlock(&log_mutex);
   return 0;
}

void flushlog()
{
   int i;
   char nullval[MAX_LOG_LENGTH];

   // Khóa mutex để truy cập vùng găng
   pthread_mutex_lock(&log_mutex);
   
   // Nếu buffer rỗng, không cần flush
   if (count == 0) {
       pthread_mutex_unlock(&log_mutex);
       return;
   }

   printf("flushlog()\n");
   sprintf(nullval, "%d", -1);
   for (i = 0; i < count; i++)
   {
      printf("Slot  %i: %s\n", i, logbuf[i]);
      strcpy(logbuf[i], nullval);
   }

   fflush(stdout);

   /*Reset buffer */
   count = 0;
   // Báo hiệu buffer không còn đầy
   pthread_cond_broadcast(&buffer_not_full);
   pthread_mutex_unlock(&log_mutex);
   return;

}

void *timer_start(void *args)
{
   while (1)
   {
      flushlog();
      /*Waiting until the next timeout */
      usleep(((struct _args *) args)->interval);
   }
}

int main()
{
   int i;
   count = 0;
   pthread_t tid[MAX_LOOPS];
   pthread_t lgrid;
   int id[MAX_LOOPS];

   struct _args args;
   args.interval = 500e3;
   /*500 msec ~ 500 * 1000 usec */

   /*Setup periodically invoke flushlog() */
   pthread_create(&lgrid, NULL, &timer_start, (void*) &args);

   /*Asynchronous invoke task writelog */
   for (i = 0; i < MAX_LOOPS; i++)
   {
      id[i] = i;
      pthread_create(&tid[i], NULL, wrlog, (void*) &id[i]);
   }

   for (i = 0; i < MAX_LOOPS; i++)
      pthread_join(tid[i], NULL);

   sleep(5);

   return 0;
}
