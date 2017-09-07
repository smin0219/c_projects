#include <criterion/criterion.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfmm.h"

/**
 *  HERE ARE OUR TEST CASES NOT ALL SHOULD BE GIVEN STUDENTS
 *  REMINDER MAX ALLOCATIONS MAY NOT EXCEED 4 * 4096 or 16384 or 128KB
 */

Test(sf_memsuite, Malloc_an_Integer, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(sizeof(int));
  *x = 4;
  cr_assert(*x == 4, "Failed to properly sf_malloc space for an integer!");
}

Test(sf_memsuite, Free_block_check_header_footer_values, .init = sf_mem_init, .fini = sf_mem_fini) {
  void *pointer = sf_malloc(sizeof(short));
  sf_free(pointer);
  pointer = (char*)pointer - 8;
  sf_header *sfHeader = (sf_header *) pointer;
  cr_assert(sfHeader->alloc == 0, "Alloc bit in header is not 0!\n");
  sf_footer *sfFooter = (sf_footer *) ((char*)pointer + (sfHeader->block_size << 4) - 8);
  cr_assert(sfFooter->alloc == 0, "Alloc bit in the footer is not 0!\n");
}

Test(sf_memsuite, SplinterSize_Check_char, .init = sf_mem_init, .fini = sf_mem_fini){
  void* x = sf_malloc(32);
  void* y = sf_malloc(32);
  (void)y;

  sf_free(x);

  x = sf_malloc(16);

  sf_header *sfHeader = (sf_header *)((char*)x - 8);
  cr_assert(sfHeader->splinter == 1, "Splinter bit in header is not 1!");
  cr_assert(sfHeader->splinter_size == 16, "Splinter size is not 16");

  sf_footer *sfFooter = (sf_footer *)((char*)sfHeader + (sfHeader->block_size << 4) - 8);
  cr_assert(sfFooter->splinter == 1, "Splinter bit in header is not 1!");
}

Test(sf_memsuite, Check_next_prev_pointers_of_free_block_at_head_of_list, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4);
  memset(x, 0, 0);
  cr_assert(freelist_head->next == NULL);
  cr_assert(freelist_head->prev == NULL);
}

Test(sf_memsuite, Coalesce_no_coalescing, .init = sf_mem_init, .fini = sf_mem_fini) {
  int *x = sf_malloc(4);
  int *y = sf_malloc(4);
  memset(y, 0, 0);
  sf_free(x);

  //just simply checking there are more than two things in list
  //and that they point to each other
  cr_assert(freelist_head->next != NULL);
  cr_assert(freelist_head->next->prev != NULL);
}

//#
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//#

Test(sf_memsuite, Malloc_an_big_memory, .init = sf_mem_init, .fini = sf_mem_fini){
  int *x = sf_malloc(7000);
  memset(x,0,0);
  cr_assert(((freelist_head->header.block_size)<<4)==1168,"Block size is not correct!!!!!");
}

Test(sf_memsuite, Check_sf_info, .init = sf_mem_init, .fini = sf_mem_fini){
  int *x = sf_malloc(7000);
  int *y = sf_malloc(200);
  int *z = sf_malloc(700);
  int *t = sf_malloc(32);
  int *n = sf_malloc(200);

  sf_free(y);
  sf_free(t);

  info test;
  sf_info(&test);

  memset(x,0,0);
  memset(y,0,0);
  memset(z,0,0);
  memset(t,0,0);
  memset(n,0,0);

  cr_assert(test.allocatedBlocks==3,"Number of allocated blocks are not correct");
  cr_assert(test.splinterBlocks==0,"Number of splinter blocks are not correct");
  cr_assert(test.padding==20,"Padding size is not correct");
  cr_assert(test.splintering==0,"Splintering size is not correct");
  cr_assert(test.coalesces==2,"Number of coalesces are not correct");
}

Test(sf_memsuite, Max_size_of_allocation , .init = sf_mem_init, .fini = sf_mem_fini){
  int *x = sf_malloc(4080);
  int *y = sf_malloc(4080);
  int *z = sf_malloc(4080);
  int *t = sf_malloc(4080);

  sf_free(x);
  sf_free(y);
  sf_free(z);
  sf_free(t);

  memset(x,0,0);
  memset(y,0,0);
  memset(z,0,0);
  memset(t,0,0);

  cr_assert(freelist_head != NULL,"Free list is empty!!!");
  cr_assert(freelist_head->header.block_size<<4==16384,"Number of allocated blocks are not correct");
}

Test(sf_memsuite, check_split , .init = sf_mem_init, .fini = sf_mem_fini){
     info inform;
    sf_info(&inform);

    int *x = sf_malloc(32);
    //sf_free(v1);
    int *y = sf_malloc(32);
    int *z = sf_malloc(9000);

    sf_free(y);

    memset(x,0,0);
    memset(y,0,0);
    memset(z,0,0);

    sf_malloc(16);
    sf_info(&inform);

    cr_assert(inform.allocatedBlocks==3,"Number of allocated blocks are not correct");
    cr_assert(inform.splinterBlocks==1,"Number of splinter blocks are not correct");
    cr_assert(inform.padding==8,"Padding size is not correct");
    cr_assert(inform.splintering==16,"Splintering size is not correct");
    cr_assert(inform.coalesces==2,"Number of coalesces are not correct");
}
