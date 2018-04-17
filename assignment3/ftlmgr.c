//#define PRINT_FOR_DEBUG

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

int backup_blk = BLOCKS_PER_DEVICE - 1;
bool pbn_flag[BLOCKS_PER_DEVICE];

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void initialize_flash_memory();
void print_block(int pbn);
void print_blkmaptbl();

int dd_read(int ppn, char *pagebuf);
int dd_write(int ppn, char *pagebuf);
int dd_erase(int pbn);

bool is_full();
int is_same_lsn(int lsn);
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
	memset(pbn_flag, false, sizeof(pbn_flag));
	return;
}

bool is_full() {
	int i;
	for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
		if(i == backup_blk)
			continue;
		
		if(!pbn_flag[i])
			return false;
	}
	return true;
}

int copy_block(int dest_pbn, int src_pbn) {
	char *pagebuf = (char *)malloc(PAGE_SIZE);
	SpareData *sdata = (SpareData *)malloc(SPARE_SIZE);

	int i, j;
	int dest_first_ppn =  PAGES_PER_BLOCK * dest_pbn;
	int src_first_ppn =  PAGES_PER_BLOCK * src_pbn;
	
	j = 0;
	for(i = 0; i < SECTORS_PER_BLOCK; i++) {
		memset(pagebuf, 0, PAGE_SIZE);
		dd_read(src_first_ppn + i, pagebuf);
		
		memset(sdata, 0, SPARE_SIZE);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		if(sdata->invalid == -1) {
			dd_write(dest_first_ppn + j, pagebuf);
			j++;
		}
	}

	free(pagebuf);
	free(sdata);
	return j;
}

int is_same_lsn(int lsn) {
	int i;
	int lbn = lsn / SECTORS_PER_BLOCK;
	int pbn = blkmaptbl.entry[lbn].pbn;
	int first_ppn = pbn * PAGES_PER_BLOCK;
	int first_free_page_offset = blkmaptbl.entry[lbn].first_free_page_offset;

	char *pagebuf = (char *)malloc(PAGE_SIZE);
	SpareData *sdata = (SpareData *)malloc(SPARE_SIZE);

	for(i = first_ppn; i < first_ppn + first_free_page_offset; i++) {
		dd_read(i, pagebuf);

		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		if(sdata->invalid == -1) {
			if(lsn == sdata->lsn)
				return i;
		}
	}	

	free(pagebuf);
	free(sdata);
	return -1;		
}

void ftl_write(int lsn, char *sectorbuf)
{
	int lbn = lsn / SECTORS_PER_BLOCK;
	int i;
	int ppn;

	char *pagebuf = (char *)malloc(PAGE_SIZE);
	SpareData *sdata = (SpareData *)malloc(SPARE_SIZE);

	if(blkmaptbl.entry[lbn].pbn == -1) {
		for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
			if(!pbn_flag[i]) {
				if(i == backup_blk)
					continue;

				pbn_flag[i] = true;
				blkmaptbl.entry[lbn].pbn = i;

				memset(pagebuf, 0xFF, PAGE_SIZE);
				memcpy(pagebuf, sectorbuf, SECTOR_SIZE);

				memset((char *)sdata, 0xFF, SPARE_SIZE);
				sdata->lsn = lsn;
				sdata->invalid = -1;
				memcpy(pagebuf+SECTOR_SIZE, sdata, SPARE_SIZE);

				ppn = blkmaptbl.entry[lbn].first_free_page_offset + PAGES_PER_BLOCK * blkmaptbl.entry[lbn].pbn;
				blkmaptbl.entry[lbn].first_free_page_offset++;
				dd_write(ppn, pagebuf);
				break;
			}
		}
	}
	else if(blkmaptbl.entry[lbn].first_free_page_offset < PAGES_PER_BLOCK) {
		if((ppn = is_same_lsn(lsn)) != -1) {
			dd_read(ppn, pagebuf);	
			memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
			sdata->invalid = 1;
			memcpy(pagebuf+SECTOR_SIZE, sdata, SPARE_SIZE);
			dd_write(ppn, pagebuf);
		}			

		memset(pagebuf, 0xFF, PAGE_SIZE);
		memcpy(pagebuf, sectorbuf, SECTOR_SIZE);

		memset((char *)sdata, 0xFF, SPARE_SIZE);
		sdata->lsn = lsn;
		sdata->invalid = -1;
		memcpy(pagebuf+SECTOR_SIZE, sdata, SPARE_SIZE);
	
		ppn = blkmaptbl.entry[lbn].first_free_page_offset + PAGES_PER_BLOCK * blkmaptbl.entry[lbn].pbn;
		blkmaptbl.entry[lbn].first_free_page_offset++;
		dd_write(ppn, pagebuf);
	}
	else if(blkmaptbl.entry[lbn].first_free_page_offset >= PAGES_PER_BLOCK) {
		if((ppn = is_same_lsn(lsn)) != -1) {
			dd_read(ppn, pagebuf);		
			memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
			sdata->invalid = 1;
			memcpy(pagebuf+SECTOR_SIZE, sdata, SPARE_SIZE);
			dd_write(ppn, pagebuf);
		}			

		if(is_full()) {
			blkmaptbl.entry[lbn].first_free_page_offset = copy_block(backup_blk, blkmaptbl.entry[lbn].pbn);
			pbn_flag[backup_blk] = true;
			pbn_flag[blkmaptbl.entry[lbn].pbn] = false;

			dd_erase(blkmaptbl.entry[lbn].pbn);

			blkmaptbl.entry[lbn].pbn = backup_blk;
			backup_blk = blkmaptbl.entry[lbn].pbn;	

			memset(pagebuf, 0xFF, PAGE_SIZE);
			memcpy(pagebuf, sectorbuf, SECTOR_SIZE);

			memset((char *)sdata, 0xFF, SPARE_SIZE);
			sdata->lsn = lsn;
			sdata->invalid = -1;

			memcpy(pagebuf+SECTOR_SIZE, sdata, SPARE_SIZE);
			ppn = blkmaptbl.entry[lbn].first_free_page_offset + PAGES_PER_BLOCK * blkmaptbl.entry[lbn].pbn;
			blkmaptbl.entry[lbn].first_free_page_offset++;
			dd_write(ppn, pagebuf);
		}
		else {
			for(i = 0; i < BLOCKS_PER_DEVICE; i++) {
				if(!pbn_flag[i]) {
					if(i == backup_blk)
						continue;

					blkmaptbl.entry[lbn].first_free_page_offset = copy_block(i, blkmaptbl.entry[lbn].pbn);

					pbn_flag[i] = true;
					pbn_flag[blkmaptbl.entry[lbn].pbn] = false;

					dd_erase(blkmaptbl.entry[lbn].pbn);
					
					blkmaptbl.entry[lbn].pbn = i;

					memset(pagebuf, 0xFF, PAGE_SIZE);
					memcpy(pagebuf, sectorbuf, SECTOR_SIZE);

					memset((char *)sdata, 0xFF, SPARE_SIZE);
					sdata->lsn = lsn;
					sdata->invalid = -1;
					
					memcpy(pagebuf+SECTOR_SIZE, sdata, SPARE_SIZE);
					ppn = blkmaptbl.entry[lbn].first_free_page_offset + PAGES_PER_BLOCK * blkmaptbl.entry[lbn].pbn;
					blkmaptbl.entry[lbn].first_free_page_offset++;
					dd_write(ppn, pagebuf);
					
					break;
				}
			}
		}
	}

#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_blkmaptbl();

	for(i = 0; i < BLOCKS_PER_DEVICE; i++) 
		print_block(i);
#endif
	free(pagebuf);
	free(sdata);

	return;
}

void ftl_read(int lsn, char *sectorbuf)
{
	int lbn = lsn / PAGES_PER_BLOCK;	
	int pbn = blkmaptbl.entry[lbn].pbn;
	int first_ppn = pbn * PAGES_PER_BLOCK;
	char pagebuf[PAGE_SIZE];
	SpareData sdata[SPARE_SIZE];
	int ppn;
	int i;
	
	for(i = 0; i < PAGES_PER_BLOCK; i++) {
		ppn = first_ppn + i;
		dd_read(ppn, pagebuf);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		if(sdata->invalid == -1 && sdata->lsn == lsn) {
			memcpy(sectorbuf, pagebuf, SECTOR_SIZE);	
			return;
		}
	}

	fprintf(stderr, "There's no data mapping with logical sector number %d\n", lsn);
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
