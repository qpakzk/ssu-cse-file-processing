#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "record.h"

#define SIZE 200

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
	const char *filename = "student.dat";
	int fd;
	int num;
	Student st, *student;
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

		printf("Input student's id : ");
		input(st.id);
		write(fd, st.id, sizeof(st.id) - 1);

		printf("Input student's name : ");
		input(st.name);
		write(fd, st.name, sizeof(st.name) - 1);

		printf("Input student's address : ");
		input(st.address);
		write(fd, st.address, sizeof(st.address) - 1);

		printf("Input student's university : ");
		input(st.univ);
		write(fd, st.univ, sizeof(st.univ) - 1);

		printf("Input student's department : ");
		input(st.dept);
		write(fd, st.dept, sizeof(st.dept) - 1);

		printf("Input others : ");
		input(st.others);
		write(fd, st.others, sizeof(st.others) - 1);
	}
	
	close(fd);

	if((fd = open(filename, O_RDONLY)) < 0) {
		fprintf(stderr, "open() error for %s\n", filename);
		exit(1);
	}	
	
	student = (Student *)calloc(num, sizeof(Student));
	lseek(fd, -SIZE * num, SEEK_END);

	for(i = 0; i < num; i++) {
		read(fd, student[i].id, sizeof(student[i].id) - 1);
		read(fd, student[i].name, sizeof(student[i].name) - 1);
		read(fd, student[i].address, sizeof(student[i].address) - 1);
		read(fd, student[i].univ, sizeof(student[i].univ) - 1);
		read(fd, student[i].dept, sizeof(student[i].dept) - 1);
		read(fd, student[i].others, sizeof(student[i].others) - 1);
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
