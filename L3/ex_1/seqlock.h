#ifndef PTHREAD_H
#include <pthread.h>
#endif

#include <stdatomic.h>

typedef struct pthread_seqlock {
    atomic_uint seq;
    pthread_mutex_t lock;
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw) {
    atomic_init(&rw->seq, 0);
    pthread_mutex_init(&rw->lock, NULL);
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    atomic_fetch_add_explicit(&rw->seq, 1, memory_order_relaxed); // Begin write (odd)
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw) {
    atomic_fetch_add_explicit(&rw->seq, 1, memory_order_release); // End write (even)
    pthread_mutex_unlock(&rw->lock);
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw) {
    unsigned seq1;
    do {
        seq1 = atomic_load_explicit(&rw->seq, memory_order_acquire);
        if (seq1 & 1) continue; // Writer in progress
        __sync_synchronize(); // Memory barrier
    } while (seq1 != atomic_load_explicit(&rw->seq, memory_order_acquire));
    return 1;
}

static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw) {
    // No-op in seqlock, because reader does not acquire a lock
    return 0;
}
