#include "debug.h"
#include "arraylist.h"
#include "foreach.h"

static pthread_key_t key = 0;
static pthread_once_t once_key = PTHREAD_ONCE_INIT;

static void create_key(){
	(void) pthread_key_create(&key,NULL);
}


void *foreach_init(arraylist_t *self){

    void *ret = NULL;
    int *index= 0;

    ret = get_index_al(self,0);

    pthread_mutex_lock(&self->lock);

    (void)pthread_once(&once_key,create_key);
    index = (int*)calloc(1,sizeof(int));
    *index = 0;
    pthread_setspecific(key,index);

    if(ret==NULL){
    	pthread_mutex_unlock(&self->lock);
    	return NULL;
    }
    else{
    	pthread_mutex_unlock(&self->lock);
    	return ret;
    }
}

void *foreach_next(arraylist_t *self, void* data){
	void* ret = NULL;
	int* index = 0;
	int result = 0;

	pthread_mutex_lock(&self->lock);

	index = pthread_getspecific(key);

	if(data == NULL){
		info("DATA IS NULL!%s","\n");
		pthread_mutex_unlock(&self->lock);
		return (char*)self->base+(*index)*self->item_size;
	}
	if(*((char*)self->base+(*index)*self->item_size)==0){
		//info("ARRAYLIST IS EMPTY%s","\n");
		pthread_mutex_unlock(&self->lock);
		return NULL;
	}
	if(memcmp((char*)self->base+(*index)*self->item_size,data,self->item_size)!=0){
		memmove((char*)self->base+(*index)*self->item_size,data,self->item_size);
		*index+=1;
		ret = (char*)self->base+(*index)*self->item_size;
		if(*((char*)self->base+(*index)*self->item_size)==0){
			info("ARRAYLIST IS EMPTY%s","\n");
			pthread_mutex_unlock(&self->lock);
			return NULL;
		}
		pthread_setspecific(key,index);
		pthread_mutex_unlock(&self->lock);
		return ret;
	}
	else{
		*index+=1;
		ret = (char*)self->base+(*index)*self->item_size;
		if(*((char*)self->base+(*index)*self->item_size)==0){
			info("ARRAYLIST IS EMPTY%s","\n");
			pthread_mutex_unlock(&self->lock);
			return NULL;
		}
		result = pthread_setspecific(key,index);
		if(result != 0){
			error("UNCOMMON ERROR FROM FOREACH_NEXT!!%s","\n");
			pthread_mutex_unlock(&self->lock);
			return NULL;
		}
		pthread_mutex_unlock(&self->lock);
		return ret;
	}
		error("UNCOMMON ERROR FROM FOREACH_NEXT!!%s","\n");
		pthread_mutex_unlock(&self->lock);
		return NULL;
}

size_t foreach_index(){
    //size_t ret = UINT_MAX;
    int* index = 0;
    index = pthread_getspecific(key);
    if(index==NULL){
    	return UINT_MAX;
    }
    else{
    	return *index;
    }
}

bool foreach_break_f(){
    bool ret = false;
    //delete_al()
    ret = true;
    return ret;
}

int apply(arraylist_t *self, int (*application)(void*)){
    int ret = 0;
    void* value = NULL;
    value = foreach_init(self);
    //pthread_mutex_lock(&self->lock);
    if(value==NULL){
    	error("ERROR OCCURRED!!%s","\n");
    	//pthread_mutex_unlock(&self->lock);
    	return -1;
    }
    //pthread_mutex_unlock(&self->lock);
    while(value!=NULL){
    	pthread_mutex_lock(&self->lock);
    	ret = application(value);
    	pthread_mutex_unlock(&self->lock);
    	if(ret==-1){
    		//pthread_mutex_unlock(&self->lock);
    		value = NULL;
    	}
    	//pthread_mutex_unlock(&self->lock);
    	value = foreach_next(self,value);
    }
    //pthread_mutex_unlock(&self->lock);
    return 0;
}

/* ####################3 HELPER ###################### */

int numbers(void* data){
	printf("G");
	return 0;
}
