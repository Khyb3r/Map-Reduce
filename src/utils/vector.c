#include "vector.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

vector_t* vector_init(size_t element_size) {
    vector_t* vector = (vector_t*) malloc(sizeof(vector_t));
    vector->size = 0;
    vector->capacity = INITIAL_CAPACITY;
    vector->element_size = element_size;
    vector->data = malloc(vector->capacity * element_size);
    if (vector->data == NULL) {
        free(vector->data);
        fprintf(stderr, "Not enough memory");
        exit(1);
    }
    return vector;
}

void vector_destroy(vector_t* vector) {
    free(vector->data);
    free(vector);
}

void vector_push_back(vector_t* v, void* elem) {
    // Resize vector as size is same as capacity
    if (v->size == v->capacity) {
        v->capacity *= 2;
        void* temp = realloc(v->data, v->capacity * v->element_size);
        assert(temp != NULL);
        v->data = temp;
    }
    void* ptr = (char*) v->data + v->size * v->element_size;
    memcpy(ptr, elem, v->element_size);
    v->size += 1;
}

void* vector_top(vector_t* v) {
    if (v->size == 0) return NULL;
    return (char*) v->data + (v->size - 1) * v->element_size;
}

void vector_pop_back(vector_t* v) {
    // Disregard capacity
    assert(v->size != 0);
    v->size -= 1;
}

void vector_pop_front(vector_t* v, void* out) {
    if (v->size == 0) return;
    // Get first element
    memcpy(out, v->data, v->element_size);
    // Shift everything left
    if (v->size > 1) {
        memmove(v->data, (char*) v->data + v->element_size, (v->size - 1) * v->element_size);
    }
    v->size--;
}