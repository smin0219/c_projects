#include "arraylist.h"


/**
 * @visibility HIDDEN FROM USER
 * @return     true on success, false on failure
 */

static bool resize_al(arraylist_t* self){

    int size = 0;
    int new_capacity = 0;
    //error("LENGTH: %lu\n",self->length);
    //error("CAPACITY: %lu\n",self->capacity);


    /* GROWING ARRAYLIST SIZE */

    //pthread_mutex_lock(&self->lock);
    if(self->length >= self->capacity){

        new_capacity = self->capacity*2;

        size = (new_capacity*self->item_size);

        /* RESIZE ARRAYLIST TO BIGGER SIZE */
        self->base = realloc(self->base,size);

        /* INITIALIZE OF GROWN PART OF ARRAYLIST TO ZERO */
        //memset((char*)self->base+(size/2),0,(size/2));

        if(self->base==NULL){
            errno = ENOMEM;
            error("Realloc failed!%s","\n");
            return false;
        }
        else{
            self->capacity *= 2;
            return true;
        }
    }

    /* SHRINGKING ARRAYLIST SIZE */
    if(self->length<=(self->capacity/2)-1){
        //pthread_mutex_lock(&lock);
        new_capacity = self->capacity/2;
        /* IF ARRAYLIST IS SMALLER THAN INIT_SZ */
        if(new_capacity<INIT_SZ){
            info("capacity is below INIT_SZ%s","\n");
            return true;
        }
        else{
            size = (new_capacity*self->item_size);
            info("shrinking array!%s","\n");
            self->base = realloc(self->base,size);

            if(self->base==NULL){
                error("Realloc failed!%s","\n");
                errno = ENOMEM;
               // pthread_mutex_unlock(&lock);
                return false;
            }
            else{
                self->capacity = new_capacity;
                //pthread_mutex_unlock(&lock);
                return true;
            }
        }
    }

    /* DO NOT NEED TO BE RESIZED */
    info("Do not need to be resized!%s","\n");
    info("Same size and address of array will be returned%s","\n");
    //pthread_mutex_unlock(&self->lock);

    return true;
}

arraylist_t *new_al(size_t item_size){

    /* ALLOCATE MEMORY FOR A NEW INSTANCE OF AN ARRAYLIST. */
	arraylist_t *arr_list = (arraylist_t*)malloc(sizeof(*arr_list));

    /* INITIALIZE ALL VARIABLES OF ARRAYLIST_T STRUCT TO DEFAULT */
    arr_list->capacity = INIT_SZ;
    arr_list->length = 0;
    arr_list->item_size = item_size;
    arr_list->base = calloc(INIT_SZ,item_size);
    pthread_mutex_init(&arr_list->lock,NULL);

    /* IF CALLOC RETURNS NULL */
	if(arr_list == NULL)
        error("Calloc failed!%s","\n");
    	errno = ENOMEM;

    info("Return value from new_al: %p\n",arr_list);
    return arr_list;
}

size_t insert_al(arraylist_t *self, void* data){
    size_t ret = UINT_MAX;
    int index = 0;
    bool is_resized = false;
    void* empty_slot = NULL;

    //
    /* IF ARRAYLIST CAPACITY IS ZERO */
    if(self->capacity==0){
        errno = EINVAL;
        error("Capacity is zero!%s","\n");
        return ret;
    }

    pthread_mutex_lock(&self->lock);

    if(self->length>=self->capacity){

        is_resized = resize_al(self);

        if(is_resized == false){
            errno = ENOMEM;
            pthread_mutex_unlock(&self->lock);
            return ret;
        }
    }
    index = self->length;

    empty_slot = ((char*)self->base)+(index*self->item_size);
    self->length++;
    memmove(empty_slot,data,self->item_size);

    pthread_mutex_unlock(&self->lock);
    return index;
}

size_t get_data_al(arraylist_t *self, void *data){
    size_t ret = UINT_MAX;
    int index = 0;
    int count = 0;
    int result = 0;

    pthread_mutex_lock(&self->lock);

    count = self->length;

    /* IF (data == NULL), RETURN THE INDEX OF FIRST ITEM IN ARRAYLIST */
    if(data == NULL){
        /* FIND INDEX OF FIRST ITEM */
        pthread_mutex_unlock(&self->lock);
        return 0;
    }

    while(count!=0){
        if(strlen(data)>self->item_size){
            error("Invalid argument!%s","\n");
            error("Bigger than item_size%s","\n");
            errno = EINVAL;
            return ret;
        }
        result = memcmp((char*)data,((char*)self->base+index*(self->item_size)),self->item_size);
        if(result==0){
            pthread_mutex_unlock(&self->lock);
            return index;
        }
        index++;
    }
    errno = EINVAL;
    pthread_mutex_unlock(&self->lock);
    return ret;
}

void *get_index_al(arraylist_t *self, size_t index){

    void* ret = NULL;
    pthread_mutex_lock(&self->lock);
    ret = calloc(1,self->item_size);

    /* IF INDEX IS GREATER THAN CAPACITY */
    if(index>self->length){
        info("Index is greater than length of arraylist%s","\n");
        memmove(ret,(char*)self->base+(self->length-1),self->item_size);
        if(ret==NULL){
            errno = EINVAL;
            pthread_mutex_unlock(&self->lock);
            return NULL;
        }
        pthread_mutex_unlock(&self->lock);
        return (char*)self->base+(self->length-1)*self->item_size;
    }
    /* IF ARRAY LIST IS EMPTY AT PROVIDED INDEX */
    if((char*)self->base+index*(self->item_size)==0){
        error("Arraylist is empty at index: %lu\n",index);
        errno = EINVAL;
        pthread_mutex_unlock(&self->lock);
        return NULL;
    }
    /* NORMAL CASE */
    //info("Value of index in the arraylist: %s\n",(char*)self->base+index*(self->item_size));
    memmove(ret,(char*)self->base+index*(self->item_size),self->item_size);
    pthread_mutex_unlock(&self->lock);

    return ret;
}

bool remove_data_al(arraylist_t *self, void *data){
    //bool ret = false;

    bool is_removed = false;
    int index = 0;
    //int count = 0;
    pthread_mutex_lock(&self->lock);
    if(data==NULL){
        memset((char*)self->base+index*self->item_size,0,self->item_size);
        is_removed = true;
    }

    else{
        while(index<self->length){
            if(memcmp((char*)data,((char*)self->base+index*self->item_size),self->item_size)==0){
                memset((char*)self->base+index*self->item_size,0,self->item_size);
                is_removed = true;
                break;
            }
            else{
                index++;
            }
        }
    }
    if(is_removed==false){
        error("No match found to remove data!%s","\n");
        errno = EINVAL;
        pthread_mutex_unlock(&self->lock);
        return false;
    }
    else{
        self->length--;
    }

    /* NOW MOVE DOWN IN MEM */
    while(index<self->length){
        memmove(((char*)self->base+index*self->item_size),((char*)self->base+(index+1)*self->item_size),self->item_size);
        memset((char*)self->base+(index+1)*self->item_size,0,self->item_size);
        index++;
    }

    if(self->length <= (self->capacity/2)-1){
        resize_al(self);
    }
    pthread_mutex_unlock(&self->lock);
    return true;
}

void *remove_index_al(arraylist_t *self, size_t index){
    void *ret = 0;
    int c_index = 0;
    pthread_mutex_lock(&self->lock);
    ret = calloc(1,self->item_size);

    /* IF INDEX IS GREATER THAN CAPACITY */
    if(index>self->length-1){
        info("Index is greater than length of arraylist%s","\n");
        memmove(ret,(char*)self->base+(self->length-1),self->item_size);
        memset((char*)self->base+(self->length-1),0,self->item_size);
        if(ret==NULL){
            errno = EINVAL;
            pthread_mutex_unlock(&self->lock);
            return NULL;
        }
        self->length--;
        pthread_mutex_unlock(&self->lock);
        return ret;
    }
    /* IF ARRAY LIST IS EMPTY AT PROVIDED INDEX */
    if((char*)self->base+index*(self->item_size)==0){
        error("Arraylist is empty at index: %lu\n",index);
        errno = EINVAL;
        pthread_mutex_unlock(&self->lock);
        return NULL;
    }
    /* NORMAL CASE */
    info("Value of index in the arraylist: %s\n",(char*)self->base+index*(self->item_size));

    memmove(ret,(char*)self->base+index*(self->item_size),self->item_size);
    memset((char*)self->base+index*self->item_size,0,self->item_size);
    self->length--;

    c_index = index;

    while(c_index<self->length){
        memmove(((char*)self->base+index*self->item_size),((char*)self->base+(index+1)*self->item_size),self->item_size);
        memset((char*)self->base+(index+1)*self->item_size,0,self->item_size);
        c_index++;
    }

    if(self->length <= (self->capacity/2)-1){
        resize_al(self);
    }
    pthread_mutex_unlock(&self->lock);
    return ret;
}

void delete_al(arraylist_t *self, void (*free_item_func)(void*)){
    int index = 0;

    if(free_item_func!=NULL){
        while(index<self->length){
            free_item_func((char*)self->base+index*self->item_size);
            index++;
        }
        free(self->base);
        self->length = 0;
        self->capacity = 0;
    }
    else{
        free(self->base);
        self->length = 0;
        self->capacity = 0;
    }
}

/* ##################### HELPER FUNCTIONS ######################## */

int empty_index(arraylist_t *self){
    int index = 0;
    int count = 0;

    count = self->capacity;

    if(self->length == 0){
        return index;
    }

    while(count!=0){
        if(*((char*)self->base+index*(self->item_size)) != 0){
            index++;
            count--;
        }
        else{
            return index;
        }
    }
    error("NO EMPTY SPACE EXIST!%s","\n");
    return -1;
}
