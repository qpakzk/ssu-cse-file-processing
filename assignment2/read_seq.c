#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include "record.h"

#define RECORD_SIZE 200

typedef struct _Record {
    char id[10];
    char name[20];
    char addr[50];
    char univ[20];
    char dept[30];
    char others[70];
} Record;

int main(int argc, char **argv) 
{
	int fd;
	int num_of_records;
	Record record;
	const char *filename = "student.dat";

	unsigned sz_id = sizeof(record.id),
			 sz_name = sizeof(record.name),
			 sz_addr = sizeof(record.addr),
			 sz_univ = sizeof(record.univ),
			 sz_dept = sizeof(record.dept),
			 sz_others = sizeof(record.others);
	int i;
	time_t start_time, end_time, elapsed_time;
	struct timeval start_val, end_val;

	if(argc != 2) {
		fprintf(stderr, "usage: %s [ RECORD_COUNT ]\n", argv[0]);
		exit(1);
	}

	num_of_records = atoi(argv[1]);

	if((fd = open(filename, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}	

	lseek(fd, (off_t) 0, SEEK_SET);

	gettimeofday(&start_val, NULL);	
	for(i = 0; i < num_of_records; i++)
		read(fd, &record, RECORD_SIZE);
	gettimeofday(&end_val, NULL);
	
	start_time = start_val.tv_sec * 1000000 + start_val.tv_usec;
	end_time = end_val.tv_sec * 1000000 + end_val.tv_usec;
	elapsed_time = end_time - start_time;
	
	printf("elasped_time: %ld us\n", elapsed_time);

	close(fd);
	return 0;
}
