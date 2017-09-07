#ifndef HELPER_H
#define HELPER_H

/* CREATE FREE BLOCK */
void create_freeheader(struct sf_header* new_header, int block_size);

/* (RE)CREATE HEADER ACCORING TO THE BLOCK SIZE */
void alloc_header(struct sf_header* header, int block_size, int padding_size, int splinter_size);

/* (RE)CREATE FOOTER ACOORING TO THE BLOCK SIZE */
void create_footer(void* footer_addr, int alloc, int splinter, int block_size);


void coalescing(struct sf_free_header* front_freeblock, struct sf_free_header* back_freeblock);

/* CHECK WHETHER THERE EXIST SPLINTERS OR NOT */
int check_splinter(int orig_block, int new_block);

void clean_header(struct sf_header* header);

void clean_footer(struct sf_footer* footer);

void freelist_position(struct sf_free_header* curr_freelist,struct sf_free_header* new_freelist);


#endif
