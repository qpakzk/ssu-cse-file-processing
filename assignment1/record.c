#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "record.h"

#define SIZE 200

//read, write 시 record를 200바이트 맞춰줌
typedef struct _Record
{
	char id[10];
	char name[20];
	char addr[50];
	char univ[20];
	char dept[30];
	char others[70];

} Record;

//표준 입력 함수, 입력의 최대 크기 제한 설정
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

int main(int argc, char **argv)
{
	int fd;
	int num;
	Record record;
	Student st, *student;
	const char *filename = "student.dat";

	unsigned sz_id = sizeof(record.id),
			 sz_name = sizeof(record.name),
			 sz_addr = sizeof(record.addr),
			 sz_univ = sizeof(record.univ),
			 sz_dept = sizeof(record.dept),
			 sz_others = sizeof(record.others);
	int i;
	unsigned file_sz, total_record_cnt;

	if(argc != 2) {
		fprintf(stderr, "usage: %s [ RECORD_COUNT ]\n", argv[0]);
		exit(1);
	}

	num = atoi(argv[1]);

	if((fd = open(filename, O_WRONLY | O_CREAT, 0666)) < 0) {
		fprintf(stderr, "open() error for %s\n", filename);
		exit(1);
	}

	lseek(fd, 0, SEEK_END);
	
	//st로 입력받은 후 record로 write할 데이터를 저장
	//200바이트로 한꺼번에 write 수행하여 I/O 횟수를 최소화
	for(i = 0; i < num; i++) {
		memset(&st, 0, sizeof(Student));
		memset(&record, 0, sizeof(Record));

		printf("Input student's id (max size = %d) : ", sz_id);
		input(st.id, sz_id);
		strncpy(record.id, st.id, sz_id);

		printf("Input student's name (max size = %d) : ", sz_name);
		input(st.name, sz_name);
		strncpy(record.name, st.name, sz_name);

		printf("Input student's address (max size = %d) : ", sz_addr);
		input(st.address, sz_addr);
		strncpy(record.addr, st.address, sz_addr);

		printf("Input student's university (max size = %d) : ", sz_univ);
		input(st.univ, sz_univ);
		strncpy(record.univ, st.univ, sz_univ);

		printf("Input student's department (max size = %d) : ", sz_dept);
		input(st.dept, sz_dept);
		strncpy(record.dept, st.dept, sz_dept);

		printf("Input others (max size = %d) : ", sz_others);
		input(st.others, sz_others);
		strncpy(record.others, st.others, sz_others);

		write(fd, &record, SIZE);
	}

	close(fd);

	if((fd = open(filename, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}	
	
	file_sz = lseek(fd, 0L, SEEK_END);
	total_record_cnt = file_sz / SIZE;

	student = (Student *)calloc(num, sizeof(Student));
	lseek(fd, -SIZE * num, SEEK_END);

	//200바이트로 한꺼번에 read 수행하여 I/O 횟수를 최소화
	//record로 받은 데이터를 student에 저장
	for(i = 0; i < num; i++) {
		memset(&record, 0, sizeof(Record));
		read(fd, &record, SIZE);

		strncpy(student[i].id, record.id, sz_id);
		strncpy(student[i].name, record.name, sz_name);
		strncpy(student[i].address, record.addr, sz_addr);
		strncpy(student[i].univ, record.univ, sz_univ);
		strncpy(student[i].dept, record.dept, sz_dept);
		strncpy(student[i].others, record.others, sz_others);
	}

	//read한 record 데이터 출력
	printf("\nRead and print recently stored records in %s\n", filename);
	for(i = 0; i < num; i++) {
		printf("Data in Record %d\n", total_record_cnt - num + 1 + i);
		printf("id : %s\n", student[i].id);
		printf("name : %s\n", student[i].name);
		printf("address : %s\n", student[i].address);
		printf("university : %s\n", student[i].univ);
		printf("department : %s\n", student[i].dept);
		printf("others : %s\n\n", student[i].others);
	}

	free(student);
	close(fd);

	return 0;
}
