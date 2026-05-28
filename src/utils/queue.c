#include "queue.h"

file_queue_t* file_queue_init(size_t element_size, size_t number_of_files) {
    file_queue_t* file_queue = (file_queue_t* ) malloc(sizeof(file_queue_t));
    int rc = pthread_mutex_init(&file_queue->lock, NULL);
    assert(rc == 0);
    file_queue->no_of_files = number_of_files;
    file_queue->queue = vector_init(element_size);
    return file_queue;
}

inline void file_queue_destroy(file_queue_t* fq) {
    pthread_mutex_destroy(&fq->lock);
    free(fq->queue);
    free(fq);
}

