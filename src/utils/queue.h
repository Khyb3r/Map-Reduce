#ifndef QUEUE_H
#define QUEUE_H
#include <assert.h>
#include <stdlib.h>

#include "vector.h"
#include "pthread.h"

typedef struct {
    pthread_mutex_t lock;
    size_t no_of_files;
    vector_t* queue;
} file_queue_t;

file_queue_t* file_queue_init(size_t, size_t);

inline void file_queue_destroy(file_queue_t*);

char* pop_from_queue(file_queue_t*);

#endif //QUEUE_H
