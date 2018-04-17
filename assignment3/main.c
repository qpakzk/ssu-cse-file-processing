#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blkmap.h"

void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void initialize_flash_memory();
void print_block(int pbn);
void print_blkmaptbl();
extern FILE *devicefp;

void print_sector(int lsn, char *sectorbuf);

int main(int argc, char *argv[])
{
	FILE *workloadfp;
	char sectorbuf[SECTOR_SIZE];
	int lsn;
	int i;
	int test;

	devicefp = fopen("flashmemory", "w+b");
	if(devicefp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}

	initialize_flash_memory();

	ftl_open();
	
	scanf("%d", &test);

	switch(test) {
		case 0:
			for(i = 0; i < PAGES_PER_BLOCK * DATABLKS_PER_DEVICE; i++) {
				sprintf(sectorbuf, "data %d", i);	
				ftl_write(i, sectorbuf);

				print_sector(i, sectorbuf);		
			}

			sprintf(sectorbuf, "data 124");	
			ftl_write(5, sectorbuf);
			print_sector(5, sectorbuf);		

			sprintf(sectorbuf, "data 125");	
			ftl_write(10, sectorbuf);
			print_sector(10, sectorbuf);		

			sprintf(sectorbuf, "data 126");	
			ftl_write(100, sectorbuf);
			print_sector(100, sectorbuf);		
			
			break;
		case 1:
			sprintf(sectorbuf, "data 0");	
			ftl_write(10, sectorbuf);
			print_sector(10, sectorbuf);		

			sprintf(sectorbuf, "data 1");	
			ftl_write(11, sectorbuf);
			print_sector(11, sectorbuf);		

			sprintf(sectorbuf, "data 2");	
			ftl_write(10, sectorbuf);
			print_sector(10, sectorbuf);		
			
			sprintf(sectorbuf, "data 3");	
			ftl_write(9, sectorbuf);
			print_sector(9, sectorbuf);		
			
			sprintf(sectorbuf, "data 4");	
			ftl_write(9, sectorbuf);
			print_sector(9, sectorbuf);		
			
			sprintf(sectorbuf, "data 5");	
			ftl_write(100, sectorbuf);
			print_sector(100, sectorbuf);		

			sprintf(sectorbuf, "data 6");	
			ftl_write(8, sectorbuf);
			print_sector(8, sectorbuf);		
			
			sprintf(sectorbuf, "data 7");	
			ftl_write(101, sectorbuf);
			print_sector(101, sectorbuf);		

			sprintf(sectorbuf, "data 8");	
			ftl_write(8, sectorbuf);
			print_sector(8, sectorbuf);		
			
			sprintf(sectorbuf, "data 9");	
			ftl_write(120, sectorbuf);
			print_sector(120, sectorbuf);		

			break;	
			
	}
	print_blkmaptbl();
	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
		print_block(i);

	fclose(devicefp);

	return 0;
}

void print_sector(int lsn, char *sectorbuf) {
	memset(sectorbuf, 0xFF, SECTOR_SIZE);
	ftl_read(lsn, sectorbuf);
	printf("[%d %s]\n", lsn, sectorbuf);
}
