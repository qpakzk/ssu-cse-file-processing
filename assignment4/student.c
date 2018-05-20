#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "student.h"

typedef struct _Node {
	short rrn;
	struct _Node *next;
} Node;

typedef struct _Header {
	short num_of_records;
	short head_num;
} Header;

const char *filename;
Node *head;

void set_linked_list(FILE *fp);

Header read_header(FILE *fp);
void write_header(FILE *fp, Header *header);

STUDENT store_student(char *argv[]);

void pack(char *recordbuf, const STUDENT *s);
void unpack(const char *recordbuf, STUDENT *s);
void read(FILE *fp, char *recordbuf, int rrn);
int find_keyval(FILE *fp, const char *keyval);
void remove_node(Header *header);
void add(FILE *fp, const STUDENT *s);
int search(FILE *fp, const char *keyval);
void delete(FILE *fp, const char *keyval);
void printRecord(const STUDENT *s, int n);
void retrieve_list(FILE *fp);
void retrieve_list(FILE *fp);
void retrieveAllRecords(FILE *fp);

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
	set_linked_list(fp);

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
	retrieve_list(fp);
	retrieveAllRecords(fp);

	fclose(fp);
	fp = NULL;

	return 1;
}

void set_linked_list(FILE *fp) {
	Header header;
	short rrn;
	char recordbuf[RECORD_SIZE];
	Node *tail;
	Node *newNode;
	
	header = read_header(fp);
	rrn = header.head_num;
	if(rrn == -1)
		head = tail = NULL;
	else {
		memset(recordbuf, 0x00, RECORD_SIZE);
		read(fp, recordbuf, rrn);
		rrn = (recordbuf[1] & 0xFF) + ((recordbuf[2] << 2) & 0xFF00);
		head = (Node *)malloc(sizeof(Node));
		head->rrn = rrn;
		head->next = NULL;
		tail = head;
		while(rrn != -1) {
			memset(recordbuf, 0x00, RECORD_SIZE);
			read(fp, recordbuf, rrn);
			rrn = (recordbuf[1] & 0xFF) + ((recordbuf[2] << 2) & 0xFF00);
			newNode = (Node *)malloc(sizeof(Node));
			newNode->rrn = rrn;
			newNode->next = NULL;
			tail->next = newNode;	
			tail = newNode;
		}
	}
}

Header read_header(FILE *fp)
{
	Header header;
	char headerbuf[HEADER_SIZE];

	fseek(fp, 0, SEEK_SET);
	fread(headerbuf, 1, HEADER_SIZE, fp);
	header.num_of_records = (headerbuf[0] & 0xFF) + ((headerbuf[1] << 2) & 0xFF00);
	header.head_num = (headerbuf[2] & 0xFF) + ((headerbuf[3] << 2) & 0xFF00);

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

int find_keyval(FILE *fp, const char *keyval)
{
	char id[ID_SIZE + 1];
	int file_size;
	int count_record;
	int i;
	
	fseek(fp, 0, SEEK_END);	
	file_size = ftell(fp);
	count_record = (file_size - HEADER_SIZE) / RECORD_SIZE;
	memset(id, 0x00, ID_SIZE + 1);
	for(i = 0; i < count_record; i++) {
		memset(id, 0x00, ID_SIZE + 1);
		fseek(fp, HEADER_SIZE + RECORD_SIZE * i, SEEK_SET);
		fread(id, 1, ID_SIZE, fp);
		if(!strcmp(keyval, id)) {
			return i;
		}
	}
	return -1;
}

void remove_node(Header *header) {	
	Node *temp;

	header->head_num = head->rrn;
	temp = head;
	head = head->next;
	free(temp);
	temp = NULL;
}

void add(FILE *fp, const STUDENT *s)
{
	char recordbuf[RECORD_SIZE];
	Header header;
	short rrn;

	if(find_keyval(fp, s->id) != -1) {
		fprintf(stderr, "ID %s duplicates in %s.\n", s->id, filename);
		exit(1);	
	}
	pack(recordbuf, s);
	
	header = read_header(fp);
	if(header.head_num == -1)
		fseek(fp, 0, SEEK_END);
	else {
		rrn = header.head_num;
		remove_node(&header);
		fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET); 
	}

	fwrite(recordbuf, 1, RECORD_SIZE, fp);
	header.num_of_records++;
	write_header(fp, &header);
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

void delete(FILE *fp, const char *keyval) {
	STUDENT student;
	char recordbuf[RECORD_SIZE];
	short rrn;
	Header header;
	Node *newNode;

	if((rrn = find_keyval(fp, keyval)) == -1) {
		fprintf(stderr, "There is no ID %s in %s.\n", keyval, filename);
		exit(1);
	}
	
	header = read_header(fp);
	memset(recordbuf, 0x00, RECORD_SIZE);

	recordbuf[0] = '*';
	recordbuf[1] = header.head_num & 0xFF;
	recordbuf[2] = (header.head_num >> 2) & 0xFF;
		
	newNode = (Node *)malloc(sizeof(Node));
	newNode->rrn = header.head_num;
	header.head_num = rrn;
	newNode->next = head;
	head = newNode;
	header.num_of_records--;
	write_header(fp, &header);
	
	fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
	fwrite(recordbuf, 1, RECORD_SIZE, fp);
}

void retrieve_list(FILE *fp) {
	Header header;
	Node *temp;
	int count_node = 0;

	header = read_header(fp);
	printf(">> Header Information <<\n");
	printf("Number of Records = %d\n", header.num_of_records);
	printf("Head Number = %d\n", header.head_num);

	printf(">> Linked List BEGIN <<\n");
	temp = head;

	while(temp != NULL) {
		printf("node %d : %d\n", count_node++, temp->rrn);	
		temp = temp->next;
	}	
	printf(">> Linked List END <<\n");
}

void retrieveAllRecords(FILE *fp) {
	int fsize;
	int record_count;
	int i;
	char recordbuf[RECORD_SIZE];
	short rrn;
	
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);

	record_count = (fsize - HEADER_SIZE) / RECORD_SIZE;
	
	printf(">> Begin to retrieve all records <<\n");
	for(i = 0; i < record_count; i++) {
		memset(recordbuf, 0x00, RECORD_SIZE);
		read(fp, recordbuf, i);
		if(recordbuf[0] == '*') {
			rrn = (recordbuf[1] & 0xFF) + ((recordbuf[2] << 2) & 0xFF00);
			printf("record %d : %c%d\n", i, recordbuf[0], rrn);
		}
		else
			printf("record %d : %s\n", i, recordbuf);
	}

	printf(">> End to retrieve all records <<\n");
}

void printRecord(const STUDENT *s, int n)
{
	int i;

	for(i=0; i<n; i++)
	{
		printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}

