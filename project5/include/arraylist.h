#ifndef ARRAYLIST_H
#define ARRAYLIST_H
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <pthread.h>
#include "debug.h"
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "const.h"


typedef struct{
    size_t capacity;
    size_t length;
    size_t item_size;
    void* base;
    pthread_mutex_t lock;
}arraylist_t;

arraylist_t *new_al(size_t item_size);

size_t insert_al(arraylist_t *self, void* data);

size_t get_data_al(arraylist_t *self, void *data);

void *get_index_al(arraylist_t *self, size_t index);

bool remove_data_al(arraylist_t *self, void *data);

void *remove_index_al(arraylist_t *self, size_t index);

void delete_al(arraylist_t *self, void (*free_item_func)(void*));

/* ######################## HELPER FUNCITONS ####################### */

int empty_index(arraylist_t *self);

#endif
