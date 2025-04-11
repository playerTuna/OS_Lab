#include "seqlock.h"

int pthread_seqlock_init(pthread_seqlock_t *seqlock) {
    seqlock->sequence = 0;
    return pthread_mutex_init(&seqlock->lock, NULL);
}

int pthread_seqlock_destroy(pthread_seqlock_t *seqlock) {
    return pthread_mutex_destroy(&seqlock->lock);
}

int pthread_seqlock_wrlock(pthread_seqlock_t *seqlock) {
    pthread_mutex_lock(&seqlock->lock);
    seqlock->sequence++; // odd => writing
    return 0;
}

int pthread_seqlock_wrunlock(pthread_seqlock_t *seqlock) {
    seqlock->sequence++; // even => write done
    pthread_mutex_unlock(&seqlock->lock);
    return 0;
}

int pthread_seqlock_rdlock(pthread_seqlock_t *seqlock) {
    unsigned long seq1, seq2;
    do {
        seq1 = seqlock->sequence;
        __sync_synchronize(); // memory barrier
        seq2 = seqlock->sequence;
    } while (seq1 != seq2 || (seq1 & 1)); // retry if writing or changed
    return 0;
}

int pthread_seqlock_rdunlock(pthread_seqlock_t *seqlock) {
    // nothing to do
    return 0;
}

