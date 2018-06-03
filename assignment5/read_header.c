#include <stdio.h>
#include <stdlib.h>

#define HEADER_SIZE 4

int main(int argc, char *argv[]) {
	char *filename; 			
	FILE *fp;
	char headerbuf[HEADER_SIZE];	
	short num_of_records;
	short del_rrn;

	if(argc < 2) {
		fprintf(stderr, "USAGE: %s [FILE_NAME]\n", argv[0]);
		exit(1);
	}
	
	filename = argv[1];
	if((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}
	
	fseek(fp, 0, SEEK_SET);	
	fread(headerbuf, 1, HEADER_SIZE, fp);
	
	num_of_records = ((headerbuf[0] << 8) & 0xFF00) + (headerbuf[1] & 0xFF);
	del_rrn = ((headerbuf[2] << 8) & 0xFF00) + (headerbuf[3] & 0xFF);
	
	printf("레코드 개수 = %d\n", num_of_records);
	printf("RRN = %d\n", del_rrn);
	
	exit(0);
}
