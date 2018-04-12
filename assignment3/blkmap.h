#ifndef	_BLOCK_MAPPING_H_
#define	_BLOCK_MAPPING_H_

#define TRUE				1
#define	FALSE				0

#define	SECTOR_SIZE			512			
#define	SPARE_SIZE			16			
#define	PAGE_SIZE			(SECTOR_SIZE+SPARE_SIZE)
#define SECTORS_PER_PAGE	1
#define	PAGES_PER_BLOCK		4									// 수정가능
#define SECTORS_PER_BLOCK	(SECTORS_PER_PAGE*PAGES_PER_BLOCK)
#define	BLOCK_SIZE			(PAGE_SIZE*PAGES_PER_BLOCK)
#define	BLOCKS_PER_DEVICE	32								// 수정가능
#define	DEVICE_SIZE			(BLOCK_SIZE*BLOCKS_PER_DEVICE)
#define DATABLKS_PER_DEVICE	(BLOCKS_PER_DEVICE - 1)				// free block은 최소한 하나를 유지

//
// 필요하면 상수를 추가해서 사용 가능함
// 

typedef struct
{
	int lsn;									// physical block의 각 page에 데이터를 저장할 때 spare area에 lsn도 같이 저장함
	int invalid;
	char dummy[SPARE_SIZE - 8];
} SpareData;

typedef struct
{
	int pbn;
	int first_free_page_offset;					// 'pbn'이 가리키는 block내에서 최초의 free page의 offset
} BlkMapTblEntry;

typedef struct									// block mapping table
{
	BlkMapTblEntry entry[DATABLKS_PER_DEVICE];
} BlockMapTbl;

#endif
