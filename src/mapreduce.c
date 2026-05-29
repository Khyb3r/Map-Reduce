#include "mapreduce.h"
#include <pthread.h>
#include <stdlib.h>
#include "utils/queue.h"

typedef struct {
    Mapper map_func;
    file_queue_t* file_queue;
} args_t;


void *mapper(void* args) {
    args_t* mapper_thread = (args_t *) args;

    while (1) {
        char* file = pop_from_queue(mapper_thread->file_queue);
        // No more files left to process
        if (file == NULL) break;
        // Process file (map)
        mapper_thread->map_func(file);
    }

    return NULL;
}

typedef struct {
    void* key;
    void* value;
} key_value_t;

typedef struct {
    vector_t* values;
    pthread_mutex_t lock;
} partition_t;

void MR_Run(int argc, char *argv[], Mapper map, int num_mappers, Reducer reduce, int num_reducers, Partitioner partition) {
    // Create mapper threads that will process each file
    pthread_t mapper_threads[num_mappers];
    args_t thread_args[num_mappers];
    file_queue_t file_queue* = file_queue_init(sizeof(char*) , argc-1);

    // Populate file queue with all cli file names passed in
    for (int i = 1; i < file_queue->no_of_files; i++) {
        vector_push_back(file_queue->queue, argv[i]);
    }

    // Create mapper threads
    for (int i = 0; i < num_mappers; i++) {
        thread_args[i].map_func = map;
        thread_args[i].file_queue = file_queue;
        pthread_create(&mapper_threads[i], NULL, mapper, &thread_args[i]);
    }

    /* Intermediate data structure
     Partitions -> List of K,V
     Locks across all of the partitions */
    const unsigned int NUMBER_OF_PARTITIONS = num_reducers;
    vector_t* partitions = vector_init(sizeof(partition_t*));

    for (int i = 0; i < NUMBER_OF_PARTITIONS; i++) {
        partition_t* this_partition = malloc(sizeof(partition_t));
        this_partition->values = vector_init(sizeof(key_value_t));
        int rc = pthread_mutex_init(&this_partition->lock, NULL);
        assert(rc == 0);
        vector_push_back(partitions, this_partition);
    }


}


unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0') {
        hash = hash * 33 + c;
    }
    return hash % num_partitions;
}

void MR_Emit(char *key, char *value) {
}