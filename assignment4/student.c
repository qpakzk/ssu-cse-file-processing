#include <stdio.h>
#include <stdlib.h>
#include "student.h"

typedef struct _Node {
	int rrn;
	struct _Node *next;
} Node;

const char *filename = "student.dat";

short convert_to_short(char *header);
void insert();

void pack(char *recordbuf, const STUDENT *s);
void unpack(const char *recordbuf, STUDENT *s);
void read(FILE *fp, char *recordbuf, int rrn);
void add(FILE *fp, const STUDENT *s);
int search(FILE *fp, const char *keyval);
void delete(FILE *fp, const char *keyval);
void printRecord(const STUDENT *s, int n);

int main(int argc, char *argv[])
{
	FILE *fp;

	char headerbuf[HEADER_SIZE];
	char recordbuf[RECORD_SIZE];

	short num_of_records;
	short head_num;

	short rrn;
	Node *head;

	if((fp = fopen(filename, "r")) == NULL) {
		headerbuf[0] = 0x00;
		headerbuf[1] = 0x00;
		headerbuf[2] = 0xFF; 
		headerbuf[3] = 0xFF;
		
		if((fp = fopen(filename, "w")) == NULL) {
			fprintf(stderr, "open error for %s\n", filename);
			exit(1);
		}

		fseek(fp, 0, SEEK_SET);
		fwrite(headerbuf, 1, HEADER_SIZE, fp);
	}
	fclose(fp);
	fp = NULL;

	if((fp = fopen(filename, "a+")) == NULL) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}

	fseek(fp, 0, SEEK_SET);
	fread(headerbuf, 1, HEADER_SIZE, fp);
	num_of_records = convert_to_short(&headerbuf[0]);
	head_num = convert_to_short(&headerbuf[2]);
	
	head = NULL;
	rrn = head_num;
	while(rrn != -1) {
		read(fp, recordbuf, rrn);
		if(recordbuf[0] == '*') {
			rrn = convert_to_short(&recordbuf[1]);
			insert(head, rrn);	
		}
	}
	
	fclose(fp);
	fp = NULL;

	return 1;
}

short convert_to_short(char *buf)
{
	return (buf[0] & 0xFF) + ((buf[1] << 2) & 0xFF00);
}

void insert(Node *head, int rrn)
{
	Node *temp = (Node *)malloc(sizeof(Node));	
	temp->rrn = rrn;
	temp->next = head;
	head = temp;
}

void read(FILE *fp, char *recordbuf, int rrn)
{
	fseek(fp, HEADER_SIZE + RECORD_SIZE * rrn, SEEK_SET);
	fread(recordbuf, 1, RECORD_SIZE, fp);
}

void add(FILE *fp, const STUDENT *s)
{
		
}
void printRecord(const STUDENT *s, int n)
{
	int i;

	for(i=0; i<n; i++)
	{
		printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}
