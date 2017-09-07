#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "arraylist.h"
#include "foreach.h"


/******************************************
 *                  ITEMS                 *
 ******************************************/
arraylist_t *global_list;

typedef struct {
    char* name;
    int32_t id;
    double gpa;
}student_t;

typedef struct{
    int i;
    float f;
    long double ld;
    char c1:4;
    char c2:4;
    short s;
    void *some_data;
}test_item_t;

pthread_mutex_t lock;
int a[10];

/******************************************
 *              HELPER FUNCS              *
 ******************************************/
void test_item_t_free_func(void *argptr){
    test_item_t* ptr = (test_item_t*) argptr;
    if(ptr)
        free(ptr->some_data);
    else
        cr_log_warn("%s\n", "Pointer was NULL");
}

void setup(void) {
    cr_log_warn("Setting up test");
    global_list = new_al(sizeof(test_item_t));
}

void teardown(void) {
    cr_log_error("Tearing down");
    delete_al(global_list, NULL);
}

void* thread(void* vargp){
    //pthread_mutex_lock(&lock);
    //printf("%d: ",*(int*)vargp);
    //printf("FIRST!!\n");

    //pthread_mutex_lock(&global_list->lock);

    int a = 1;
    int b = 1;

    insert_al(global_list,&a);
    insert_al(global_list,&b);
    //remove_data_al(global_list,&a);
    //foreach_init(global_list);
    //foreach_next(global_list,&c);
    //foreach_next(global_list,&b);
    //foreach_next(global_list,&b);
    //printf("INDEX: %lu\n",foreach_index());
    apply(global_list,numbers);


    //pthread_mutex_unlock(&lock);
    return 0;
}
void* thread2(void* vargp){

    //printf("SECOND!!\n");
    pthread_mutex_lock(&global_list->lock);
    for(int i = 0; i<1000;i++){
       printf("%d ",*(int*)((char*)global_list->base+(i*global_list->item_size)));
    //printf("%s, ",((char*)global_list->base+((*(int*)vargp)-5)*global_list->item_size));
    }
    pthread_mutex_unlock(&global_list->lock);
//
        //get_data_al(global_list,&i);

    return 0;
}

/******************************************
 *                  TESTS                 *
 ******************************************/
/*
Test(al_suite, 0_creation, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(test_item_t));

    cr_assert_not_null(locallist, "List returned was NULL");
}
*/
/*
Test(al_suite, 1_deletion, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(test_item_t));

    cr_assert_not_null(locallist, "List returned was NULL");

    delete_al(locallist, test_item_t_free_func);

    cr_assert(true, "Delete completed without crashing");
}
*/
//Test(al_suite, 2_insertion, .timeout=2, .init=setup, .fini=teardown){

/*
    pthread_t tid[10];

    for(int i=0; i<10;i++){
        pthread_create(&tid[i],NULL,thread,NULL);
    }
    for (int i = 0; i < 10; ++i){
        pthread_join(tid[i],NULL);
    }
*/
    //memset(a,0,sizeof(a));
    //char* msg = "FIRST: ";
    //char* msg2 = "SECOND: ";

    //int j = 1;
/*
    pthread_t tid[2];
    pthread_create(&tid[0],NULL,thread,NULL);
    pthread_create(&tid[1],NULL,thread2,NULL);

    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);

    //for(int i=0; i<10;i++){
    //    printf("%d ",*(int*)((char*)global_list->base+(i*global_list->item_size)));
    //}

    exit(0);

    cr_assert(true, "I win");

}
*/
Test(al_suite, 3_insertion, .timeout=2, .init=setup, .fini=teardown){
    pthread_t tid[5];
    for(int i =0; i<5; i++){
        pthread_create(&tid[i],NULL,thread,NULL);
    }
    for(int i=0; i<5; i++){
        pthread_join(tid[i],NULL);
    }
    printf("LENGTH: %lu\n",global_list->length);

    //cr_assert(global_list->length==2000,"insertion failed");
}

//Test(al_suite, 3_removal, .timeout=2, .init=setup, .fini=teardown){
//}
