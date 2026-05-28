#include "mapreduce.h"
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    Mapper mapper;
    char* file_name;
} args_t;


void *mapper(void* args) {
    args_t* mapper_wrapper_args = (args_t *) args;

    mapper_wrapper_args->mapper(mapper_wrapper_args->file_name);

    return NULL;
}


void MR_Run(int argc, char *argv[], Mapper map, int num_mappers, Reducer reduce, int num_reducers, Partitioner partition) {
    // Create mapper threads that will process each file
    pthread_t mapper_threads[num_mappers];
    args_t thread_args[num_mappers];

    for (int i = 0; i < argc-1; i++) {
        thread_args[i].file_name = argv[i];
        thread_args[i].mapper = map;
        // Each mapper thread processes a single file
        if (pthread_create(&mapper_threads[i], NULL, mapper, &thread_args[i]) != 0) {
            exit(1);
        }

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