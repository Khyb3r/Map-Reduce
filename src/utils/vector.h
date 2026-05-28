#ifndef VECTOR_H
#define VECTOR_H
#include <stdio.h>
#define INITIAL_CAPACITY 20

typedef struct {
    void* data;
    size_t size;
    size_t capacity;
    size_t element_size;
} vector_t;

vector_t* vector_init(size_t);

inline void vector_destroy(vector_t*);

void vector_push_back(vector_t*, void*);

void vector_pop_back(vector_t*);

void vector_pop_front(vector_t*);

void* vector_top(vector_t* v);

#endif //VECTOR_H
