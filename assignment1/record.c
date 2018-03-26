#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "record.h"

int main(int argc, char **argv)
{
	int fd;
	int record;
	int i;
	const char *filename = "student.dat";
	Student student, *student_data;

	if(argc != 2) {
		fprintf(stderr, "usage: %s [number of records]\n", argv[0]);
		exit(1);
	}
	
	record = atoi(argv[1]);

	if((fd = open(filename, O_WRONLY | O_CREAT, 0666)) < 0) {
		fprintf(stderr, "open() error for %s\n", filename);
		exit(1);
	}

	lseek(fd, 0, SEEK_END);

	for(i = 0; i < record; i++) {
		memset(&student, 0, sizeof(Student));
		printf("id : ");
		fgets(student.id, sizeof(student.id), stdin);
		student.id[strlen(student.id) - 1] = 0;

		printf("name : ");
		fgets(student.name, sizeof(student.name), stdin);
		student.name[strlen(student.name) - 1] = 0;

		printf("address : ");
		fgets(student.address, sizeof(student.address), stdin);
		student.address[strlen(student.address) - 1] = 0;

		printf("university : ");
		fgets(student.univ, sizeof(student.univ), stdin);
		student.univ[strlen(student.univ) - 1] = 0;

		printf("department : ");
		fgets(student.dept, sizeof(student.dept), stdin);
		student.dept[strlen(student.dept) - 1] = 0;

		printf("others : ");
		fgets(student.others, sizeof(student.others), stdin);
		student.others[strlen(student.others) - 1] = 0;
		
		write(fd, &student, sizeof(Student));
	}
	
	close(fd);

	if((fd = open(filename, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);;
	}	
	
	student_data = (Student *)calloc(record, sizeof(Student));
	lseek(fd, -sizeof(Student) * record, SEEK_END);
	for(i = 0; i < record; i++)
		read(fd, &student_data[i], sizeof(Student));

	for(i = 0; i < record; i++) {
		printf("id =>  %s\n", student_data[i].id);
		printf("name =>  %s\n", student_data[i].name);
		printf("address =>  %s\n", student_data[i].address);
		printf("university =>  %s\n", student_data[i].univ);
		printf("department =>  %s\n", student_data[i].dept);
		printf("others =>  %s\n", student_data[i].others);
	}
	free(student_data);
	return 0;
}
