#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include "record.h"

#define RECORD_SIZE 200

void input(char *in, unsigned max_sz) {
	int ch;
	unsigned cnt = 0;

	while(1) {
		ch = getchar();
		if(ch == '\n') {
			if(cnt > max_sz) {
				fprintf(stderr, "ERROR: Exceed the maximun size of input\n");
				exit(1);
			}
			*in = 0;
			return;
		}
		cnt++;
		*in = ch;
		in++;
	}
}

int main(int argc, char **argv) {
	int fd;
	int num_of_records;
	Student st;
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

	if((fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
		fprintf(stderr, "open() error for %s\n", filename);
		exit(1);
	}

	lseek(fd, (off_t) 0, SEEK_SET);
	
	for(i = 0; i < num_of_records; i++) {
		memset(&st, 0, sizeof(Student));
		memset(&record, 0, sizeof(Record));
		
		input(st.id, sz_id);
		strncpy(record.id, st.id, sz_id);

		input(st.name, sz_name);
		strncpy(record.name, st.name, sz_name);

		input(st.address, sz_addr);
		strncpy(record.addr, st.address, sz_addr);

		input(st.univ, sz_univ);
		strncpy(record.univ, st.univ, sz_univ);

		input(st.dept, sz_dept);
		strncpy(record.dept, st.dept, sz_dept);
		
		input(st.others, sz_others);
		strncpy(record.others, st.others, sz_others);

		write(fd, &record, RECORD_SIZE);
	}

	close(fd);

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
