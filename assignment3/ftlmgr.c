//#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

BlockMapTbl blkmaptbl;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void initialize_flash_memory();
void print_block(int pbn);
void print_blkmaptbl();

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

	return;
}

void ftl_write(int lsn, char *sectorbuf);
{

#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_blkmaptbl_info();
#endif

	return;
}

void ftl_read(int lsn, char *sectorbuf);
{

#ifdef PRINT_FOR_DEBUG				// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_blkmaptbl_info();
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
