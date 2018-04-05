#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define SUFFLE_NUM	10000
#define RECORD_SIZE 200

typedef struct _Record {
    char id[10];
    char name[20];
    char addr[50];
    char univ[20];
    char dept[30];
    char others[70];
} Record;

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv) {
	int *read_order_list;
	int num_of_records;
	char *filename = "student.dat";
	int fd;
	Record record;
	int nth_record;
	int i;
	struct timeval start_val, end_val;
	time_t start_time, end_time, elapsed_time;

	if(argc != 2) {
		fprintf(stderr, "usage: %s [ RECORD_COUNT ]\n", argv[0]);
		exit(1);
	}	

	num_of_records = atoi(argv[1]);
	read_order_list = (int *) calloc(num_of_records, sizeof(int));
	GenRecordSequence(read_order_list, num_of_records);
	
	if((fd = open(filename, O_RDONLY)) < 0) {
		fprintf(stderr, "open() error for %s\n", filename);
		exit(1);
	}

	gettimeofday(&start_val, NULL);
	for(i = 0; i < num_of_records; i++) {
		nth_record = read_order_list[i];	
		lseek(fd, (off_t) nth_record * RECORD_SIZE, SEEK_SET);
		read(fd, &record, RECORD_SIZE);
	}
	gettimeofday(&end_val, NULL);
			
	start_time = start_val.tv_sec * 1000000 + start_val.tv_usec;
	end_time = end_val.tv_sec * 1000000 + end_val.tv_usec;
	elapsed_time = end_time - start_time;

	printf("elapsed_time: %ld us\n", elapsed_time);

	close(fd);
	free(read_order_list);
	return 0;
}

void GenRecordSequence(int *list, int n) 
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++) {
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++) {
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b) 
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
