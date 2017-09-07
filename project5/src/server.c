#include "debug.h"
#include "arraylist.h"
#include "foreach.h"
#include "stdlib.h"
#include "stdio.h"
#include <stdbool.h>
#include <criterion/criterion.h>
#include <criterion/logging.h>

typedef struct{
    int i;
    float f;
    long double ld;
    char c1:4;
    char c2:4;
    short s;
    void *some_data;
}test_item_t;

void test_item_t_free_func(void *argptr){
    test_item_t* ptr = (test_item_t*) argptr;
    if(ptr)
        free(ptr->some_data);
    else
        printf("%s\n", "Pointer was NULL");
}

int main(int argc, char *argv[]){
	arraylist_t* arr = new_al(sizeof(4));

	int a = 0;
	int b = 0;
	//int c = 4;
	void* ret = NULL;

	insert_al(arr,&a);
	insert_al(arr,&b);


	//pthread_t th1, th2;
	//char *msg1 = "FIRST!!";
	//char *msg2 = "SECOND!!";
	//memset(a,0,sizeof(a));
	//delete_al(arr,test_item_t_free_func);
	//foreach_init(arr);
	//foreach_next(arr,&a);
	apply(arr,numbers);

	//if(c){}
	if(ret){}

	//foreach_next(arr,&c);
	//foreach_next(arr,&b);
	//foreach_next(arr,&b);

	//pthread_create(&th1,NULL,myfunc1,(void*)msg1);
	//pthread_create(&th2,NULL,myfunc2,(void*)msg2);

	//pthread_join(th1,NULL);
	//pthread_join(th2,NULL);
	return 0;
//}
/*
void *myfunc1(void* ptr){
	for(int i=0; i<100; i++){
        printf("x");
        a[i] = i;
        //insert_al(global_list,&i);
    }
	return 0;
}

void *myfunc2(void* ptr){
	for(int i=0; i<100; i++){
        printf("%d",a[i]);
        //printf("Y:%d, ",*(int*)((char*)global_list->base+i*global_list->item_size));
        //get_data_al(global_list,&i);
    }
	return 0;

}
*/


}
