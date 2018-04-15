#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>
#include "blkmap.h"

BlockMapTbl blkmaptbl;
FILE *devicefp;

int backup_blk = BLOCK_PER_DEVICE - 1;
bool pbn_flag[BLOCK_PER_DEVICE];

typdef struct {
	char sector_area[SECTOR_SIZE * SECTORS_PER_PAGE];
	SpareData spare_area;
} PageData;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void initialize_flash_memory();
void print_block(int pbn);
void print_blkmaptbl();

int dd_write(int, char *);
int dd_read(int, char *);
//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다
//
void ftl_open()
{
	int i;

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		blkmaptbl.entry[i].pbn = -1;
		blkmaptbl.entry[i].first_free_page_offset = 0;
	}

	//
	// 추가적으로 필요한 작업이 있으면 수행할 것
	//
	memset(pbn_flag, false, sizeof(pbn_table));
	return;
}

bool is_full() {
	int i;
	for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
		if(i == backup_blk)
			continue;
		
		if(!pbn_table[i].flag)
			return false;
	}

	return true;
}

bool have_invalids() {
	int i;
	for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
		if(i == backup_blk)
			continue;

		if(pbn_table[i].num_of_invalids) 
			return true;
	}
	
	return false;
}

int select_block() {
	int pbn = -1;
	int i;

	for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
		if(i == backup_blk)
			continue;

		if(pbn_table[i].num_of_invalids > 0 && pbn < pbn_table[i].num_of_invalids)
			pbn = i;
	}

	return pbn;
}

int cal_ppn(int pbn) {
	int ppn;
	int i;
		
}
void copy_valid_pages(int pbn) {
	PageData page;
	int i;

	for(i = 0; i < PAGES_PER_BLOCK; i++) {
		dd_read(pbn * BLOCK_SIZE + i, (char *)&page);
		if(page.spare_data.invalid == -1) {
			dd_write(backup_blk * BLOCK_SIZE + 
		}
	SpareData spare_data;
	}
}

void erase_block() {

}

void ftl_write(int lsn, char *sectorbuf)
{
	int lbn = lsn / BLOCK_SIZE;
	int i, j;
	int pbn;
	int ppn;
	int tmp;
	PageData pagebuf;
/*
	if(is_full()) {
		if(!have_invalids()) {
			fprintf(stderr, "Full of valid data\n");
			exit(1);
		}
		
		pbn = select_block();
		copy_valid_pages(pbn);
		erase_block();
	}
	else */

	memset((char *)&pagebuf, 0, sizeof(PageData));

	if(blkmaptbl.entry[lbn].pbn == -1) {
		for(i = 0; i < BLOCKS_PER_PER_DEVICE; i++) {
			if(!pbn_flag[i]) {
				if(i == backup_blk)
					continue;

				pbn_flag[i] = true;
				blkmaptbl.entry[lbn].pbn = i;
				blkmaptbl.entry[lbn].first_free_page_offset++;

				strncpy(pagebuf.sector_area, sectorbuf, SECTOR_SIZE * SECTORS_PER_PAGE);
				pagebuf.spare_area.lsn = lsn;
				pagebuf.spare_area.invalid = -1;	
				
				ppn = (blkmaptbl.entry[lbn].first_free_page_offset - 1) + BLOCK_SIZE * blkmaptbl.entry[lbn].pbn;
				dd_write(ppn, pagebuf);
				break;
			}
		}
	}
	/*
	else if(blkmaptbl.entry[lbn].first_free_page_offset < PAGES_PER_BLOCK) {
		//
		blkmaptbl.entry[lbn].first_free_page_offset++;
	}
	else {
		for(i = 0; i < DATABLKS_PER_DEVICE; i++) {
			if() {
				
			}
		}
	}
		ppn = blkmaptbl.entry[lbn].first_free_page_offset + lbn * BLOCK_SIZE;
		blkmaptbl.entry[lbn].pbn = lbn;
		blkmaptbl.entry[lbn].first_free_page_offset += 1;
	
	else {
		for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
			if(blkmaptbl.entry[i].first_free_page_offest < PAGES_PER_BLOCK) {
				ppn = blkmaptbl.entry[i].first_free_page_offset + lbn * BLOCK_SIZE;
				blkmaptbl.entry[i].pbn = i;
				blkmaptbl.entry[i].first_free_page_offset += 1;
				break;
			}
		}
	}	
	*/
#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_blkmaptbl();
#endif

	return;
}

void ftl_read(int lsn, char *sectorbuf)
{

#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	//print_blkmaptbl();
	print_block(1);
#endif

	return;
}

//
// initialize flash memory where each byte are set to 'OxFF'
// 
void initialize_flash_memory()
{
	char *blockbuf;
	int i;

	blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, devicefp);
	}

	free(blockbuf);

	return;
}

void print_block(int pbn)
{
	char *pagebuf;
	SpareData *sdata;
	int i;
	
	pagebuf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		dd_read(i, pagebuf);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%5d][%5d]\n", i, sdata->lsn, sdata->invalid);
	}

	free(pagebuf);
	free(sdata);

	return;
}

void print_blkmaptbl()
{
	int i;

	printf("Block Mapping Table: \n");
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		if(blkmaptbl.entry[i].pbn >= 0)
		{
			printf("[%d %d %d]\n", i, blkmaptbl.entry[i].pbn, blkmaptbl.entry[i].first_free_page_offset);
		}
	}
}
