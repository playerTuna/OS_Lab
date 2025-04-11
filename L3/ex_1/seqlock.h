#ifndef SEQLOCK_H
#define SEQLOCK_H

#include <pthread.h>

typedef struct pthread_seqlock {
    volatile unsigned long sequence;
    pthread_mutex_t lock;
} pthread_seqlock_t;

int pthread_seqlock_init(pthread_seqlock_t *seqlock);
int pthread_seqlock_destroy(pthread_seqlock_t *seqlock);
int pthread_seqlock_rdlock(pthread_seqlock_t *seqlock);
int pthread_seqlock_rdunlock(pthread_seqlock_t *seqlock);
int pthread_seqlock_wrlock(pthread_seqlock_t *seqlock);
int pthread_seqlock_wrunlock(pthread_seqlock_t *seqlock);

#endif
