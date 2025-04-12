#ifndef PTHREAD_H
#include <pthread.h>
#endif

typedef struct pthread_seqlock { /* TODO: implement the structure */
   pthread_mutex_t lock;
   unsigned int seq;

} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t* rw)
{
   /* TODO: ... */
   pthread_mutex_init(&rw->lock, NULL);
   rw->seq = 0;
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t* rw)
{
   /* TODO: ... */
   pthread_mutex_lock(&rw->lock);
   rw->seq++;
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t* rw)
{
   /* TODO: ... */
   rw->seq++;
   pthread_mutex_unlock(&rw->lock);
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t* rw)
{
   /* TODO: ... */
   unsigned int seq;

   do {
      // Wait until sequence is even (no writer active)
      do {
         seq = rw->seq;
      } while (seq % 2 == 1);  // Keep waiting if sequence is odd

      // Read the data (performed in the caller's code)

      // Check if a writer modified data while we were reading
      // If seq changed, a writer came in and modified data - retry
   } while (seq != rw->seq);

   return seq;
}


static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t* rw)
{
   /* TODO: ... */
   return rw->seq; 
}

