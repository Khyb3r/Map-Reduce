#include "mapreduce.h"
#include <pthread.h>
#include <string.h>
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

// Declare as static global so MR_Emit can also access it without changing its function signature
static vector_t* partitions;
Partitioner partition_global_func;
int num_reducers_global;

void MR_Run(int argc, char *argv[], Mapper map, int num_mappers, Reducer reduce, int num_reducers, Partitioner partition) {
    partition_global_func = partition;
    num_reducers_global = num_reducers;

    // Create mapper threads that will process each file
    pthread_t mapper_threads[num_mappers];
    args_t thread_args[num_mappers];
    file_queue_t* file_queue = file_queue_init(sizeof(char*) , argc-1);

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
     Locks across every single partitions */
    const unsigned int NUMBER_OF_PARTITIONS = num_reducers;
    partitions = vector_init(sizeof(partition_t*));

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
    // Get the specific partition this K,V should go into
    unsigned long partition_number =  partition_global_func(key, num_reducers_global);
    // Create copies of K,V
    char* key_copy = malloc(sizeof(char) * strlen(key) + 1);
    char* value_copy = malloc(sizeof(char) * strlen(value) + 1);
    strcpy(key_copy, key);
    strcpy(value_copy, value);
    // Create K,V struct and add into the specific partition
    key_value_t kv_pair = {
        .key = key_copy,
        .value = value_copy
    };

    // Update the partition
    partition_t* p = ((partition_t**)partitions->data)[partition_number];
    pthread_mutex_lock(&p->lock);
    vector_push_back(partitions[partition_number].data, &kv_pair);
    pthread_mutex_unlock(&p->lock);
}
