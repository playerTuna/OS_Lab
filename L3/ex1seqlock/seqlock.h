#ifndef SEQLOCK_H
#define SEQLOCK_H

#include <pthread.h>
#include <sched.h>

typedef struct pthread_seqlock {
   pthread_mutex_t mutex;
   volatile unsigned seq;
} pthread_seqlock_t;

static inline int pthread_seqlock_init(pthread_seqlock_t* rw) {
   if (pthread_mutex_init(&rw->mutex, NULL) != 0)
      return -1;
   rw->seq = 0;
   return 0;
}

static inline int pthread_seqlock_destroy(pthread_seqlock_t* rw) {
   return pthread_mutex_destroy(&rw->mutex);
}

static inline int pthread_seqlock_wrlock(pthread_seqlock_t* rw) {
   pthread_mutex_lock(&rw->mutex);
   rw->seq++;
   return 0;
}

static inline int pthread_seqlock_wrunlock(pthread_seqlock_t* rw) {
   rw->seq++;  // change back to even
   pthread_mutex_unlock(&rw->mutex);
   return 0;
}

static inline int pthread_seqlock_rdlock(pthread_seqlock_t* rw) {
   unsigned seq;
   while (1) {
      seq = rw->seq;
      if ((seq & 1) == 0) {
         if (seq == rw->seq)
            return 1;
      }
      sched_yield();
   }
}

static inline int pthread_seqlock_rdunlock(pthread_seqlock_t* rw) {
   return 0;
}

#endif  // SEQLOCK_H