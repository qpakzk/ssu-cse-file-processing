#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "student.h"

typedef struct _Node {
	int rrn;
	struct _Node *next;
} Node;

typedef struct _Header {
	short num_of_records;
	short head_num;
} Header;

const char *filename;
Node *head;

short convert_to_short(char *header);
Header read_header(FILE *fp);
void write_header(FILE *fp, Header *header);

STUDENT store_student(char *argv[]);

void pack(char *recordbuf, const STUDENT *s);
void unpack(const char *recordbuf, STUDENT *s);
void read(FILE *fp, char *recordbuf, int rrn);
void read_keyval(FILE *fp, char *keyval, int rrn);
int find_keyval(FILE *fp, const char *keyval);
void add(FILE *fp, const STUDENT *s);
int search(FILE *fp, const char *keyval);
void delete(FILE *fp, const char *keyval);
void printRecord(const STUDENT *s, int n);

int main(int argc, char *argv[])
{
	FILE *fp;
	char headerbuf[HEADER_SIZE];
	char recordbuf[RECORD_SIZE];
	Header header;
	short rrn;
	STUDENT student;
	char keyval[ID_SIZE + 1];

	if(argc < 3) {
		fprintf(stderr, "Usage: %s {-a|-s|-d} FILENAME field_value1 [...]\n", argv[0]);
		exit(1);
	}
	
	filename = argv[2];

	if((fp = fopen(filename, "r")) == NULL) {
		header.num_of_records = 0;
		header.head_num = -1;

		if((fp = fopen(filename, "w")) == NULL) {
			fprintf(stderr, "open error for %s\n", filename);
			exit(1);
		}
		write_header(fp, &header);
	}
	fclose(fp);
	fp = NULL;

	if((fp = fopen(filename, "r+")) == NULL) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}
	
	if(argc < 3) {
		fprintf(stderr, "Usage: %s {-a|-s|-d} %s field_value1 [...]\n", argv[0], filename);
		exit(1);
	}

	if(!strcmp(argv[1], "-a")) {
		if(argc != 10) {
			fprintf(stderr, "Usage: %s %s %s ID NAME DEPT YEAR ADDR PHONE EMAIL\n", argv[0], argv[1], filename);
			exit(1);
		}

		student = store_student(argv);
		add(fp, &student);
	}
	else if(!strcmp(argv[1], "-s")) {
		if(argc != 4) {
			fprintf(stderr, "Usage: %s %s %s ID\n", argv[0], argv[1], filename);
			exit(1);	
		}

		memset(keyval, 0x00, ID_SIZE + 1);
		memcpy(keyval, argv[3], ID_SIZE);
		search(fp, keyval);
	}
	else if(!strcmp(argv[1], "-d")) {
		if(argc != 4) {
			fprintf(stderr, "Usage: %s %s %s ID\n", argv[0], argv[1], filename);
			exit(1);	
		} 

		memset(keyval, 0x00, ID_SIZE + 1);
		memcpy(keyval, argv[3], ID_SIZE);
		delete(fp, keyval);
	}

	fclose(fp);
	fp = NULL;

	return 1;
}

short convert_to_short(char *buf)
{
	return (buf[0] & 0xFF) + ((buf[1] << 2) & 0xFF00);
}

Header read_header(FILE *fp)
{
	Header header;
	char headerbuf[HEADER_SIZE];

	fseek(fp, 0, SEEK_SET);
	fread(headerbuf, 1, HEADER_SIZE, fp);
	header.num_of_records = convert_to_short(&headerbuf[0]);
	header.head_num = convert_to_short(&headerbuf[2]);

	return header;
}

void write_header(FILE *fp, Header *header)
{
	char headerbuf[HEADER_SIZE];
	headerbuf[0] = header->num_of_records & 0xFF;
   	headerbuf[1] = (header->num_of_records >> 2) & 0xFF;
   	headerbuf[2] = header->head_num & 0xFF;
   	headerbuf[3] = (header->head_num >> 2) & 0xFF;
	fseek(fp, 0, SEEK_SET);
	fwrite(headerbuf, 1, HEADER_SIZE, fp);
}

STUDENT store_student(char *argv[]) 
{
	STUDENT student;
	
	memset(&student, 0x00, sizeof(STUDENT));
	memcpy(student.id, argv[3], ID_SIZE);
	memcpy(student.name, argv[4], NAME_SIZE);
	memcpy(student.dept, argv[5], DEPT_SIZE);
	memcpy(student.year, argv[6], YEAR_SIZE);
	memcpy(student.addr, argv[7], ADDR_SIZE);
	memcpy(student.phone, argv[8], PHONE_SIZE);
	memcpy(student.email, argv[9], EMAIL_SIZE);

	return student;
}

void pack(char *recordbuf, const STUDENT *s)
{
	memset(recordbuf, 0x00, RECORD_SIZE);
	memcpy(recordbuf, s->id, ID_SIZE);
	strncat(recordbuf, "#", 1);
	strncat(recordbuf, s->name, NAME_SIZE);
	strncat(recordbuf, "#", 1);
	strncat(recordbuf, s->dept, DEPT_SIZE);
	strncat(recordbuf, "#", 1);
	strncat(recordbuf, s->year, YEAR_SIZE);
	strncat(recordbuf, "#", 1);
	strncat(recordbuf, s->addr, ADDR_SIZE);
	strncat(recordbuf, "#", 1);
	strncat(recordbuf, s->phone, PHONE_SIZE);
	strncat(recordbuf, "#", 1);
	strncat(recordbuf, s->email, EMAIL_SIZE);
	strncat(recordbuf, "#", 1);
}

int field_size(const char *fieldbuf) {

	int size = 0;
	while(1) {
		if(fieldbuf[size] == '#')
			break;
		size++;
	}
	return size;
}

void unpack(const char *recordbuf, STUDENT *s) {
	int idx;
	int size;
	char temp[RECORD_SIZE];

	memset(s, 0x00, sizeof(STUDENT)); 

	idx = 0;
	size = field_size(recordbuf);
	memset(temp, 0x00, RECORD_SIZE); 
	memcpy(temp, recordbuf, size);
	memcpy(s->id, temp, ID_SIZE);
		
	idx += (size + 1);	
	size = field_size(&recordbuf[idx]);
	memset(temp, 0x00, RECORD_SIZE); 
	memcpy(temp, &recordbuf[idx], size);
	memcpy(s->name, temp, NAME_SIZE);

	idx += (size + 1);	
	size = field_size(&recordbuf[idx]);
	memset(temp, 0x00, RECORD_SIZE); 
	memcpy(temp, &recordbuf[idx], size);
	memcpy(s->dept, temp, DEPT_SIZE);

	idx += (size + 1);	
	size = field_size(&recordbuf[idx]);
	memset(temp, 0x00, RECORD_SIZE); 
	memcpy(temp, &recordbuf[idx], size);
	memcpy(s->year, temp, YEAR_SIZE);

	idx += (size + 1);	
	size = field_size(&recordbuf[idx]);
	memset(temp, 0x00, RECORD_SIZE); 
	memcpy(temp, &recordbuf[idx], size);
	memcpy(s->addr, temp, ADDR_SIZE);

	idx += (size + 1);	
	size = field_size(&recordbuf[idx]);
	memset(temp, 0x00, RECORD_SIZE); 
	memcpy(temp, &recordbuf[idx], size);
	memcpy(s->phone, temp, PHONE_SIZE);

	idx += (size + 1);	
	size = field_size(&recordbuf[idx]);
	memset(temp, 0x00, RECORD_SIZE); 
	memcpy(temp, &recordbuf[idx], size);
	memcpy(s->email, temp, EMAIL_SIZE);
}

void read(FILE *fp, char *recordbuf, int rrn)
{
	fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
	fread(recordbuf, 1, RECORD_SIZE, fp);
}

void read_keyval(FILE *fp, char *keyval, int rrn) {
	fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
	memset(keyval, 0x00, ID_SIZE);
	fread(keyval, 1, ID_SIZE, fp);
}

int find_keyval(FILE *fp, const char *keyval)
{
	char id[ID_SIZE + 1];
	int file_size;
	int count_record;
	int i;
	
	fseek(fp, 0, SEEK_END);	
	file_size = ftell(fp);
	count_record = (file_size - HEADER_SIZE) / RECORD_SIZE;
	for(i = 0; i < count_record; i++) {
		read_keyval(fp, id, i);
		if(!strcmp(keyval, id)) {
			return i;
		}
	}
	return -1;
}

void add(FILE *fp, const STUDENT *s)
{
	char recordbuf[RECORD_SIZE];
	if(find_keyval(fp, s->id) != -1) {
		fprintf(stderr, "ID %s duplicates in %s.\n", s->id, filename);
		exit(1);	
	}
	pack(recordbuf, s);

	fseek(fp, 0, SEEK_END);
	fwrite(recordbuf, 1, RECORD_SIZE, fp);
}

int search(FILE *fp, const char *keyval)
{
	STUDENT student;
	char recordbuf[RECORD_SIZE];
	int rrn;

	if((rrn = find_keyval(fp, keyval)) == -1) {
		fprintf(stderr, "There is no ID %s in %s.\n", keyval, filename);
		exit(1);
	}
	read(fp, recordbuf, rrn);
	unpack(recordbuf, &student);

	printRecord(&student, 1); 

	return rrn;
}

void delete(FILE *fp, const char *keyval)
{
	STUDENT student;
	char recordbuf[RECORD_SIZE];
	int rrn;

	if((rrn = find_keyval(fp, keyval)) == -1) {
		fprintf(stderr, "There is no ID %s in %s.\n", keyval, filename);
		exit(1);
	}

	memset(recordbuf, 0x00, RECORD_SIZE);
	recordbuf[0] = '*';
	
	fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
	fwrite(recordbuf, RECORD_SIZE, 1, fp);
}

void printRecord(const STUDENT *s, int n)
{
	int i;

	for(i=0; i<n; i++)
	{
		printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}

