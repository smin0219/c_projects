
#include "sfmm.h"
#include "helper.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


sf_free_header* freelist_head = NULL;
static void* start_heap = NULL;
static void* end_heap = NULL;
static void* last_page = NULL;
/* FOR SF_INFO */
static size_t allocatedBlocks = 0;
static size_t splinterBlocks = 0;
static size_t padding = 0;
static size_t splintering = 0;
static size_t coalesces = 0;
static int total_heap = 0;
static double payload = 0;
static double highest_payload = 0;
static double peakMemoryUtilization = 0;
void *sf_malloc(size_t size) {
	int block_size = 0;
	int freeblock_size = 0;
	int requested_size = 0;
	int splinter_size = 0;
	int padding_size = 0;
	void* footer_addr = NULL;
	void* payload_addr = NULL;
	void* ptr_addr = NULL;
	/* ALLOCATE BLOCK */
	sf_free_header* curr_freeblock = NULL;
	sf_free_header* smallest_freeblock = NULL;
	sf_free_header* new_freeblock = NULL;
	sf_free_header* new_page = NULL;
	//sf_free_header* next_block = NULL;
	sf_free_header* prev_block = NULL;
	sf_footer* prev_footer = NULL;
	if(size==0){
		errno = EINVAL;
		return NULL;
	}
	/* CREATE NEW HEADER AND FOOTER NODES */
	//sf_header header
	/* SET FREE_LIST FOR THE FIRST TIME */
	if(freelist_head==NULL){
			/* REQUEST HEAP SPACE FOR ALLOCATOR */
		if(sf_sbrk(0)==0){
			freelist_head = sf_sbrk(1);
		}
		if(sf_sbrk(0)!=last_page){
			freelist_head = sf_sbrk(1);
			total_heap += 4096;
		}
		else{
			errno = ENOMEM;
			return NULL;
		}
		if(start_heap == NULL){
			/* SAVE STARTING ADDRESS OF HEAP */
			start_heap = freelist_head;
			last_page = (char*)start_heap + 16384;
		}
		end_heap = sf_sbrk(0);
		/* SET ALL THE VALUES FOR HEADER OF BLOCK */
		create_freeheader(&(freelist_head->header),4096);
		/* CALCULATE FOOTER ADDRESS */

		footer_addr = ((char*)freelist_head+((freelist_head->header.block_size<<4)-8));
		/* SET ALL THE VALUES FOR FOOTER OF BLOCK */
		create_footer(footer_addr,0,0,freelist_head->header.block_size<<4);
		/* START FINDING SMALLEST FREE BLOCK FROM HEAD */
		curr_freeblock = freelist_head;
	}
	else{
		curr_freeblock = freelist_head;
	}
	/* CALCULATE REQUESTED SIZE */
	requested_size = size;
	/* CALCULATE PADDING SIZE */
	if(size%16!=0){
		padding_size = 16-(size%16);
	}
	/* CALCULATE BLOCK SIZE */
	block_size = (SF_HEADER_SIZE + SF_FOOTER_SIZE + requested_size + padding_size);
	/* FIND RIGHT SPACE TO ALLOCATE FREE BLOCK */
	do{
		/* WHEN BLOCK SIZE IS EQUAL TO THE CURRENT FREE BLOCK SIZE */
		if(block_size==((curr_freeblock->header).block_size)<<4){
			footer_addr = (char*)curr_freeblock+((curr_freeblock->header.requested_size)+8);
			payload += requested_size;
			alloc_header(&curr_freeblock->header,block_size,padding_size,0);
			create_footer(footer_addr,1,0,block_size);
			smallest_freeblock = curr_freeblock;
			if(smallest_freeblock->next != NULL){
				smallest_freeblock->next->prev = smallest_freeblock->prev;
				if(smallest_freeblock->prev != NULL){
					smallest_freeblock->prev->next = smallest_freeblock->next;
				}
				else{
					smallest_freeblock->next->prev = NULL;
					freelist_head = curr_freeblock->next;
				}
			}
			else{
				if(smallest_freeblock->prev != NULL){
					smallest_freeblock->prev->next = NULL;
				}
				else{
					freelist_head = NULL;
				}
			}
			curr_freeblock = NULL;
		}
		/* WHEN CURRENT FREE BLOCK SIZE IS BIGGER THAN BLOCK SIZE*/
		else if(block_size<((curr_freeblock->header).block_size<<4)){
			/* IF SMALLEST BLOCK PTR IS EMPTY */
			if(smallest_freeblock == NULL){
				smallest_freeblock = curr_freeblock;
			}
			else{
				/* IF NEW SMALLEST FREE BLOCK IS FOUND FROM THE LIST */
				if((((smallest_freeblock->header).block_size)<<4) > ((curr_freeblock->header).block_size<<4)){
					smallest_freeblock = curr_freeblock;
				}
			}
			curr_freeblock = curr_freeblock->next;
		}
		else{
			/* IF THERE ARE FREE BLOCKS IN THE LIST */
			if(curr_freeblock->next != NULL){
				curr_freeblock = curr_freeblock->next;
			}
			/* IF THERE IS NO ENOUGHT SIZE OF FREE BLOCKS IN THE LIST*/
			else{
				/* IF THIS IS NOT THE LAST PAGE */
				do{
					//if(sf_sbrk(0)!=last_page){
					if(sf_sbrk(0)!=last_page){
						new_page = sf_sbrk(1);
						total_heap += 4096;
					}
					else{
						errno = ENOMEM;
						return NULL;
					}
					ptr_addr = (char*)new_page-8;
					prev_footer = (sf_footer*)ptr_addr;
					freeblock_size = ((prev_footer->block_size)<<4);
					ptr_addr = (char*)new_page-freeblock_size;
					prev_block = (sf_free_header*)ptr_addr;
					create_freeheader(&new_page->header,4096);
					footer_addr = (char*)new_page+((new_page->header.requested_size)+8);
					create_footer(footer_addr,0,0,freeblock_size);
					/* COALESCING */
					if(prev_block->header.alloc == 0){
						coalesces++;
						/* SET BLOCK_SIZE FOR NEW FREE BLOCK */
						freeblock_size = ((new_page->header.block_size)<<4)+((prev_block->header.block_size)<<4);
						new_page = prev_block;
						/* CREATE NEW FREE HEADER */
						create_freeheader(&new_page->header,freeblock_size);
						/* CREATE NEW FREE FOOTER */
						footer_addr = (char*)new_page+((new_page->header.requested_size)+8);
						create_footer(footer_addr,0,0,freeblock_size);
						smallest_freeblock = new_page;
					}
					else{
						while(curr_freeblock->next != NULL){
							curr_freeblock = curr_freeblock->next;
						}
						curr_freeblock->next = new_page;
						smallest_freeblock = new_page;
					}
				}while((((smallest_freeblock->header).block_size)<<4) < block_size);
				curr_freeblock = NULL;
			}
		}
	}while(curr_freeblock!=NULL);
	/* IF THERE IS A LARGE ENOUGHT FREE BLOCK TO ALLOCATE */
	if((smallest_freeblock != NULL)&&(smallest_freeblock->header.alloc != 1)){
		/* CALCULATE SPRINTER SIZE */
		splinter_size = check_splinter((smallest_freeblock->header).block_size<<4, block_size);
		/* CALCULATE FOOTER ADDRESS */
		footer_addr = ((char*)smallest_freeblock+((block_size)-8)+splinter_size);
		/* IF THE SMALLEST FREE BLOCK IS BIGGER THAN ALLOCATED BLOCK
			THEN IT NEED TO BE SPLITTED */
		if(((smallest_freeblock->header).block_size<<4)>(block_size+splinter_size)){
			/* CREATE NEW FREE BLOCK HEADER BEFORE ALLOCATING */
			new_freeblock = (sf_free_header*)((char*)footer_addr+8);
			/* CREATE NEW HEADER */
			create_freeheader(&(new_freeblock->header),((smallest_freeblock->header.block_size)<<4)-(block_size+splinter_size));
			/* CREATE NEW FREE BLOCK FOOTER BEFORE ALLOCATING */
			footer_addr = ((char*)new_freeblock+(((new_freeblock->header.block_size)<<4)-8));
			create_footer(footer_addr,0,0,new_freeblock->header.block_size<<4);
			/* CONNECT NEW FREE BLOCK TO THE FREE BLOCK LIST */
			if(smallest_freeblock != freelist_head){
				new_freeblock->prev = smallest_freeblock->prev;
				new_freeblock->next = smallest_freeblock->next;
				if(smallest_freeblock->next != NULL){
					smallest_freeblock->next->prev = new_freeblock;
				}
				if(smallest_freeblock->prev != NULL){
					smallest_freeblock->prev->next = new_freeblock;
				}
			}
			else{
				new_freeblock->next = smallest_freeblock->next;
				if(smallest_freeblock->next != NULL){
					smallest_freeblock->next->prev = new_freeblock;
				}
				freelist_head = new_freeblock;
			}
			/* CALCULATE FOOTER ADDRESS */
			footer_addr = ((char*)smallest_freeblock+(block_size-8));
			/* NOW SPLITTING BLOCKS */
			/* CREATE HEADER AND FOOTER FOR NEWLY ALLOCATED BLOCK */
		}
		if(((smallest_freeblock->header).block_size<<4)==(block_size+splinter_size)){
			if(smallest_freeblock==freelist_head){
				freelist_head = NULL;
			}
			else{
				if(smallest_freeblock->next != NULL){
					smallest_freeblock->next->prev = smallest_freeblock->prev;
				}
				if(smallest_freeblock->prev != NULL){
					smallest_freeblock->prev->next = smallest_freeblock->next;
				}
			}
		}
		payload += requested_size;
		alloc_header(&smallest_freeblock->header,block_size,padding_size,splinter_size);
		if(splinter_size!=0){
			create_footer(footer_addr,1,1,block_size+splinter_size);
		}
		else{
			create_footer(footer_addr,1,0,block_size+splinter_size);
		}
		payload_addr = (char*)smallest_freeblock+8;
		/* HERE WE NEED TO WRITE A CODE FOR COALESCING IF THERE IS A FREE BLOCK  */
	}
	else if(smallest_freeblock->header.alloc==1){
		payload_addr = (char*)smallest_freeblock+8;
	}
	else{
		errno = ENOMEM;
		return NULL;
	}
	/* SET PROPER VALUES TO HEADER OF BLOCK */
	return payload_addr;
}
void *sf_realloc(void *ptr, size_t size) {
	void* footer_addr = NULL;
	void* ptr_addr = NULL;
	void* new_freelist = NULL;
	int splinter_size = 0;
	int block_size = 0;
	int reblock_size = 0;
	int newblock_size = 0;
	int padding_size = 0;
	//int requested_size = 0;
	//sf_free_header* new_freelist = NULL;
	sf_free_header* curr_block = NULL;
	sf_free_header* next_block = NULL;
	sf_free_header* new_block = NULL;
	//curr_freelist = freelist_head;
	/* ERROR CASES */
	if(ptr == NULL){
		errno = ENOMEM;
		return NULL;
	}
	/* INVALID PTR */
	if((ptr<start_heap)||(ptr>end_heap)){
		errno = EINVAL;
		return NULL;
	}
	/* WHEN SIZE == 0 */
	if(size == 0){
		sf_free(ptr);
		return ptr;
	}
	curr_block = (sf_free_header*)((char*)ptr-8);
	/* IF PTR IS NOT AN ALLOCATED BLOCK */
	if(curr_block->header.alloc !=1){
		errno = EINVAL;
		return NULL;
	}
	if(((char*)curr_block+((curr_block->header.block_size)<<4)) != end_heap){
		ptr_addr = (char*)curr_block+((curr_block->header.block_size)<<4);
		next_block = (sf_free_header*)ptr_addr;
	}
	if(size%16!=0){
		padding_size = 16-(size%16);
	}
	/* CALCULATE SIZE OF BLOCK TO BE REALLOCATED */
	reblock_size = size+padding_size+16;
	/* WHEN REALLOCING A BLOCK TO A SAME SIZE OF ORIG BLOCK */
	if(((curr_block->header.block_size)<<4) == reblock_size){
		return ptr;
	}
	/* WHEN REALLOCING A SMALLER BLOCK TO A ORIG BLOCK */
	if(((curr_block->header.block_size)<<4) > reblock_size){
		block_size = (curr_block->header.block_size)<<4;
		/* CHECK FOR THE SPLINTER BEFORE SPLIT */
		splinter_size = check_splinter(block_size,reblock_size);
		/* IF THERE WILL BE SPLINTER AFTER SPLIT */
		if(splinter_size!=0){
			if(next_block->header.alloc == 0){
				/* IF NEXT BLOCK IS A FREE BLOCK */
				payload -= curr_block->header.requested_size-size;
				coalesces ++;
				alloc_header(&curr_block->header,reblock_size,padding_size,0);
				footer_addr = ((char*)curr_block+((curr_block->header.block_size)<<4)-8);
				create_footer(footer_addr,1,0,reblock_size);
				ptr_addr = (char*)footer_addr+8;
				new_block = (sf_free_header*)ptr_addr;
				newblock_size = splinter_size+((next_block->header.block_size)<<4);
				create_freeheader(&new_block->header,newblock_size);
				footer_addr = ((char*)new_block+(((new_block->header.block_size)<<4)-8));
				create_footer(footer_addr,0,0,newblock_size);
				//payload-=(new_block->header.requested_size);
				if(next_block == freelist_head){
					if(next_block->next != NULL){
						new_block->next = next_block->next;
						next_block->next->prev = new_block;
					}
					else{
						new_block->next = NULL;
					}
					new_block->prev = NULL;
					freelist_head = new_block;
				}
				else{
					if(next_block->next != NULL){
						next_block->next = new_block->next;
						next_block->next->prev = new_block;
					}
					next_block->prev = new_block->prev;
					next_block->prev->next = new_block;
				}
			}
			else{
				payload -= curr_block->header.requested_size-size;
				allocatedBlocks--;
				alloc_header(&curr_block->header,reblock_size,padding_size,splinter_size);
				footer_addr = ((char*)curr_block+((curr_block->header.block_size)<<4)-8);
				create_footer(footer_addr,1,1,reblock_size+splinter_size);
			}
			return (char*)curr_block+8;
		}
		/* NO SPLINTER */
		else{
			payload -= curr_block->header.requested_size-size;
			alloc_header(&curr_block->header,reblock_size,padding_size,splinter_size);
			footer_addr = ((char*)curr_block+((curr_block->header.block_size)<<4)-8);
			create_footer(footer_addr,1,0,reblock_size+splinter_size);
			allocatedBlocks--;
			ptr_addr = (char*)footer_addr+8;
			new_block = (sf_free_header*)ptr_addr;
			newblock_size = block_size - reblock_size;
			alloc_header(&new_block->header,newblock_size,0,0);
			footer_addr = ((char*)new_block+(((new_block->header.block_size)<<4)-8));
			create_footer(footer_addr,1,0,newblock_size);
			payload += new_block->header.requested_size;
			sf_free((char*)new_block+8);
			return (char*)curr_block+8;
		}
	}
	/* WHEN REALLOCATE A BLOCK TO A LARGER BLOCK */
	if(((curr_block->header.block_size)<<4) < reblock_size){
		/* IF NEXT ADJACENT BLOCK IS A FREE BLOCK */
		if(next_block->header.alloc == 0){
			/* CALCULATE TOTAL SIZE OF BLOCK */
			block_size = ((curr_block->header.block_size)<<4)+((next_block->header.block_size)<<4);
			/* IF THE SIZE OF NEWLY COALEACED BLOCK IS BIG ENOUGH TO REALLOC */
			if(block_size>=reblock_size){
				/* CHECK FOR THE SPLINTER BEFORE REALLOC */
				splinter_size = check_splinter(block_size,reblock_size);
				/* THERE SHOULD BE NO ADJACENT FREE BLOCK, SINCE IT ALREADY CHECKED */
				if(splinter_size!=0){
					payload += size;
					alloc_header(&curr_block->header,reblock_size,padding_size,splinter_size);
					footer_addr = ((char*)curr_block+((curr_block->header.block_size)<<4)-8);
					create_footer(footer_addr,1,1,reblock_size+splinter_size);
					if(next_block->prev != NULL){
						if(next_block->next != NULL){
							next_block->prev->next = next_block->next;
						}
						else{
							next_block->prev->next = NULL;
						}
					}
					else{
						next_block->next->prev = NULL;
						freelist_head = next_block->next;
					}
				}
				else{
					payload += size;
					alloc_header(&curr_block->header,reblock_size,padding_size,splinter_size);
					footer_addr = ((char*)curr_block+((curr_block->header.block_size)<<4)-8);
					create_footer(footer_addr,1,0,reblock_size+splinter_size);
					ptr_addr = (char*)footer_addr+8;
					new_block = (sf_free_header*)ptr_addr;
					newblock_size = block_size - reblock_size;
					/* DONT NEED PADDING BECAUSE IT ALREADY ADDED TO THE PRIOR BLOCK */
					alloc_header(&new_block->header,newblock_size,0,0);
					footer_addr = ((char*)new_block+(((new_block->header.block_size)<<4)-8));
					create_footer(footer_addr,1,0,newblock_size);
					payload += new_block->header.requested_size;
					sf_free((char*)new_block+8);
				}
				return (char*)curr_block+8;
			}
			/* IF THE SIZE OF NEWLY COALESCED BLOCK IS NOT BIG ENOUGHT TO REALLOC */
			else{
				/* FIND SPACE IN FREE LIST AND ALLOCATE BLOCK TO REALLOCTE */
				new_freelist = sf_malloc(size);
				memcpy(new_freelist,(char*)curr_block+8,((curr_block->header.block_size)<<4)-16);
				sf_free((char*)curr_block+8);
				return new_freelist;
			}
		}
		/* IF NEXT ADJACENT BLOCK IS AN ALLOCATED BLOCK */
		else{
			new_freelist = sf_malloc(size);
			memcpy(new_freelist,(char*)curr_block+8,((curr_block->header.block_size)<<4)-16);
			sf_free((char*)curr_block+8);
			return new_freelist;
		}
	}
	return NULL;
}
void sf_free(void* ptr) {
	sf_free_header* new_freelist = NULL;
	sf_free_header* curr_freelist = NULL;
	sf_free_header* next_block = NULL;
	sf_free_header* prev_block = NULL;
	sf_footer* curr_footer = NULL;
	sf_footer* prev_footer = NULL;
	void* ptr_addr = NULL;
	void* footer_addr = NULL;
	int block_size = 0;
	int is_start = 0;
	int is_end = 0;
	if((ptr==NULL) || (ptr<start_heap) || (ptr>last_page)){
		errno = EINVAL;
		return;
	}
	/* HEADER OF THE BLOCK THAT NEED TO BE FREED */
	new_freelist =  (sf_free_header*)((char*)ptr-8);
	curr_freelist = freelist_head;
	ptr_addr = (char*)new_freelist+((new_freelist->header.block_size)<<4)-8;
	curr_footer = (sf_footer*)ptr_addr;
	if((new_freelist->header.alloc!=1)&&(curr_footer->alloc != 1)){
		errno = EINVAL;
		return;
	}
	if(((char*)new_freelist+((new_freelist->header.block_size)<<4)) != end_heap){
		/* FIND ADDRESS OF NEXT BLOCK HEADER */
		ptr_addr = (char*)new_freelist+((new_freelist->header.block_size)<<4);
		next_block = (sf_free_header*)ptr_addr;
	}
	else{
		is_end = 1;
	}
	if(new_freelist != start_heap){
		/* FIND ADDRESS OF PREV BLOCK HEADER */
		ptr_addr = (char*)new_freelist-8;
		prev_footer = (sf_footer*)ptr_addr;
		block_size = ((prev_footer->block_size)<<4);
		ptr_addr = (char*)new_freelist-block_size;
		prev_block = (sf_free_header*)ptr_addr;
	}
	else{
		is_start = 1;
	}
	allocatedBlocks--;
	if((new_freelist->header.splinter_size)!=0){
		splinterBlocks--;
		splintering-=(new_freelist->header.splinter_size);
	}
	if((new_freelist->header.padding_size)!=0){
		padding-=(new_freelist->header.padding_size);
	}
	payload-=new_freelist->header.requested_size;
	if(is_start==1 || is_end==1){
		if(is_start == 1){
			block_size = (new_freelist->header.block_size)<<4;
			create_freeheader(&new_freelist->header,block_size);
			footer_addr = (char*)ptr+(new_freelist->header.requested_size);
			create_footer(footer_addr,0,0,block_size);
			if(is_end!=1){
				if(next_block->header.alloc == 0){
					coalesces++;
					block_size =((new_freelist->header.block_size)<<4) + ((next_block->header.block_size)<<4);
					/* CLEAN FOOTER OF PREV BLOCK BEFORE COALESCING */
					clean_footer(footer_addr);
					/* CREATE NEW FREE HEADER */
					create_freeheader(&new_freelist->header,block_size);
					/* CREATE NEW FREE FOOTER */
					footer_addr = (char*)new_freelist+block_size-8;
					create_footer(footer_addr,0,0,block_size);
					new_freelist->next = next_block->next;
					next_block->prev = new_freelist;
					freelist_head = new_freelist;
				}
				else{
					if(freelist_head!=NULL){
						new_freelist->next = curr_freelist;
						curr_freelist->prev = new_freelist;
						freelist_head = new_freelist;
					}
					else{
						freelist_head = new_freelist;
					}
				}
			}
			freelist_head = new_freelist;
		}
		else{
			block_size = (new_freelist->header.block_size)<<4;
			create_freeheader(&new_freelist->header,block_size);
			footer_addr = (char*)ptr+(new_freelist->header.requested_size);
			create_footer(footer_addr,0,0,block_size);
			if(curr_freelist!=NULL){
				while(curr_freelist->next!=NULL){
					curr_freelist = curr_freelist->next;
				}
				if(prev_block->header.alloc == 0){
					coalesces++;
					block_size =((new_freelist->header.block_size)<<4) + ((prev_block->header.block_size)<<4);
					/* CREATE NEW FREE HEADER */
					create_freeheader(&prev_block->header,block_size);
					footer_addr = (char*)prev_block+block_size-8;
					create_footer(footer_addr,0,0,block_size);
				}
				else{
					curr_freelist->next = new_freelist;
					new_freelist->prev = curr_freelist;
				}
			}
			else{
				freelist_head = new_freelist;
			}
		}
	}
	else{
		/* CHECK PREV/NEXT BLOCKS WHETHER ITS FREE OR NOT */
		if((prev_block->header.alloc !=0) && (next_block->header.alloc !=0)){
			block_size = (new_freelist->header.block_size)<<4;
			create_freeheader(&new_freelist->header,block_size);
			footer_addr = (char*)ptr+(new_freelist->header.requested_size);
			create_footer(footer_addr,0,0,block_size);
			/* PLACE NEW FREE BLOCK TO THE FREE LIST */
			freelist_position(curr_freelist,new_freelist);
		}
		/* IF PREVIOUS BLOCK IS A FREE BLOCK */
		if(prev_block->header.alloc == 0){
			coalesces++;
			/* SET BLOCK_SIZE FOR NEW FREE BLOCK */
			block_size = ((new_freelist->header.block_size)<<4)+((prev_block->header.block_size)<<4);
			clean_header(&new_freelist->header);
			/* CLEAN FOOTER OF PREV BLOCK BEFORE COALESCING */
			clean_footer(prev_footer);
			/* SET ADDRESS OF NEW FREE BLOCK TO THE PREV FREE BLOCK */
			new_freelist = prev_block;
			/* CREATE NEW FREE HEADER */
			create_freeheader(&new_freelist->header,block_size);
			/* CREATE NEW FREE FOOTER */
			footer_addr = (char*)new_freelist+((new_freelist->header.requested_size)+8);;
			create_footer(footer_addr,0,0,block_size);
		}
		/* IF NEXT BLOCK IS A FREE BLOCK */
		if(next_block->header.alloc == 0){
			coalesces++;
			/* SET BLOCK_SIZE FOR NEW FREE BLOCK */
			block_size = ((new_freelist->header.block_size)<<4)+((next_block->header.block_size)<<4);
			clean_header(&next_block->header);
			footer_addr = (char*)new_freelist+((new_freelist->header.requested_size)+8);
			/* CLEAN FOOTER OF CURR_FREE BLOCK BEFORE COALESCING */
			clean_footer(footer_addr);
			create_freeheader(&new_freelist->header,block_size);
			footer_addr = (char*)new_freelist+((new_freelist->header.requested_size)+8);
			create_footer(footer_addr,0,0,block_size);
			if(next_block->next != NULL){
				new_freelist->next = next_block->next;
				next_block->next->prev = new_freelist;
			}
			if(next_block->prev != NULL){
				next_block->prev->next = new_freelist;
				new_freelist->prev = next_block->prev;
			}
			else{
				freelist_head = new_freelist;
			}
		}
	}
}
int sf_info(info* ptr) {
	if((sf_sbrk(0) == 0)||(ptr==NULL)){
		return -1;
	}
	ptr->allocatedBlocks = allocatedBlocks;
	ptr->splinterBlocks = splinterBlocks;
	ptr->padding = padding;
	ptr->splintering = splintering;
	ptr->coalesces = coalesces;
	ptr->peakMemoryUtilization = peakMemoryUtilization;
	return 0;
}
/****************************************************************************/
/************************** HELPER FUNCTIONS ********************************/
/****************************************************************************/
void clean_header(struct sf_header* header){
	header->alloc = 0;
	header->splinter = 0;
	header->block_size = 0;
	header->requested_size = 0;
	header->unused_bits = 0;
	header->splinter_size = 0;
	header->padding_size = 0;
}
void clean_footer(struct sf_footer* footer){
	footer->alloc = 0;
	footer->splinter = 0;
	footer->block_size = 0;
}
void create_freeheader(struct sf_header* new_header, int block_size){
	new_header->alloc = 0;
	new_header->splinter = 0;
	new_header->block_size = block_size>>4;
	new_header->requested_size = block_size - 16;
	new_header->unused_bits = 0;
	new_header->splinter_size = 0;
	new_header->padding_size = 0;
}
void create_footer(void* footer_addr, int alloc, int splinter, int block_size){
	sf_footer footer;
	footer.alloc = alloc;
	footer.splinter = splinter;
	footer.block_size = block_size>>4;
	*(sf_footer*)footer_addr = footer;
}
void alloc_header(struct sf_header* header, int block_size, int padding_size, int splinter_size){
	header->alloc = 1;
	header->block_size = (block_size+splinter_size)>>4;
	header->requested_size = block_size-padding_size-16;
	header->unused_bits = 0;
	header->splinter_size = splinter_size;
	if(header->splinter_size!=0){
		header->splinter = 1;
		splinterBlocks++;
	}
	else{
		header->splinter = 0;
	}
	header->padding_size = padding_size;
	allocatedBlocks++;
	padding += padding_size;
	splintering += splinter_size;
	//payload += header->requested_size;
	if(payload>highest_payload){
		highest_payload = payload;
	}
	peakMemoryUtilization = highest_payload/total_heap;
	//curr_header->header = header;
	//inform->allocatedBlocks=0;
}
void freelist_position(struct sf_free_header* curr_freelist,struct sf_free_header* new_freelist){
	while(curr_freelist!=NULL){
		if(new_freelist<curr_freelist){
			new_freelist->next = curr_freelist;
			curr_freelist->prev = new_freelist;
			freelist_head = new_freelist;
			curr_freelist = NULL;
		}
		else if((new_freelist>curr_freelist) && (new_freelist<curr_freelist->next)){
			new_freelist->next = curr_freelist->next;
			new_freelist->prev = curr_freelist;
			curr_freelist->next->prev = new_freelist;
			curr_freelist->next = new_freelist;
			curr_freelist = NULL;
		}
		else if(curr_freelist->next == NULL){
			curr_freelist->next = new_freelist;
			new_freelist->prev = curr_freelist;
			curr_freelist = NULL;
		}
		else{
			curr_freelist = curr_freelist->next;
		}
	}
	if(freelist_head == NULL){
		freelist_head = new_freelist;
	}
}
int check_splinter(int orig_block, int new_block){
	int result = 0;
	result = orig_block - new_block;
	if(result<32){
		//inform->splinterBlocks=0;
		return result;
	}
	else{
		return 0;
	}
}
