#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "record.h"

#define SIZE 200

typedef struct _Record
{
	char id[10];
	char name[20];
	char address[50];
	char univ[20];
	char dept[30];
	char others[70];

} Record;

void input(char *in) {
	int ch;

	while((ch = getchar()) != '\n') {
		*in = ch;
		in++;
	}
	*in = 0;
}

int main(int argc, char **argv)
{
	int fd;
	int num;
	Record record;
	Student st, *student;
	const char *filename = "student.dat";

	unsigned size_id = sizeof(record.id),
			 size_name = sizeof(record.name),
			 size_address = sizeof(record.address),
			 size_univ = sizeof(record.univ),
			 size_dept = sizeof(record.dept),
			 size_others = sizeof(record.others);
	
	int i;

	if(argc != 2) {
		fprintf(stderr, "usage: %s [number of records]\n", argv[0]);
		exit(1);
	}
	
	num = atoi(argv[1]);

	if((fd = open(filename, O_WRONLY | O_CREAT, 0666)) < 0) {
		fprintf(stderr, "open() error for %s\n", filename);
		exit(1);
	}

	lseek(fd, 0, SEEK_END);
	
	for(i = 0; i < num; i++) {
		memset(&st, 0, sizeof(Student));
		memset(&record, 0, sizeof(Record));

		printf("Input student's id : ");
		input(st.id);
		strncpy(record.id, st.id, size_id);

		printf("Input student's name : ");
		input(st.name);
		strncpy(record.name, st.name, size_name);

		printf("Input student's address : ");
		input(st.address);
		strncpy(record.address, st.address, size_address);

		printf("Input student's university : ");
		input(st.univ);
		strncpy(record.univ, st.univ, size_univ);

		printf("Input student's department : ");
		input(st.dept);
		strncpy(record.dept, st.dept, size_dept);

		printf("Input others : ");
		input(st.others);
		strncpy(record.others, st.others, size_others);

		write(fd, &record, SIZE);
	}
	
	close(fd);

	if((fd = open(filename, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}	
	
	student = (Student *)calloc(num, sizeof(Student));
	lseek(fd, -SIZE * num, SEEK_END);

	for(i = 0; i < num; i++) {
		memset(&record, 0, sizeof(Record));
		read(fd, &record, SIZE);

		strncpy(student[i].id, record.id, size_id);
		strncpy(student[i].name, record.name, size_name);
		strncpy(student[i].address, record.address, size_address);
		strncpy(student[i].univ, record.univ, size_univ);
		strncpy(student[i].dept, record.dept, size_dept);
		strncpy(student[i].others, record.others, size_others);
	}

	printf("\n");
	for(i = 0; i < num; i++) {
		printf("id =>  %s\n", student[i].id);
		printf("name =>  %s\n", student[i].name);
		printf("address =>  %s\n", student[i].address);
		printf("university =>  %s\n", student[i].univ);
		printf("department =>  %s\n", student[i].dept);
		printf("others =>  %s\n\n", student[i].others);
	}

	free(student);
	close(fd);

	return 0;
}
