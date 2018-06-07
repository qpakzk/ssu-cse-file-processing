//#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "student.h"

void readHeader(FILE *fp, char *headerbuf);
void makeRuns(FILE *inputfp, char *inputbuf);
void internalsort(char *inputbuf, int n);
void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf);
void pack(char *recordbuf, const STUDENT *s);
void unpack(const char *recordbuf, STUDENT *s);
void readChunk(FILE *runfp, char *inputbuf, int chunkid);
void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n);
void removeAllRuns();
void print_inputbuf(char *inputbuf);
void print_records(STUDENT *s, int n);

int run_count = 0;

int main(int argc, char *argv[])
{
	FILE *inputfp, *outputfp;
	char headerbuf[HEADER_SIZE];
	int num_of_records;
	char inputbuf[INPUT_BUF_SIZE];
	char outputbuf[OUTPUT_BUF_SIZE];
   	char *original_record_file;
	char *new_record_file;
	int num_of_inputs;
	int i;

	if(argc != 4) {
		fprintf(stderr, "USAGE: %s -as [RECORD_FILE] [NEW_RECORD_FILE]\n", argv[0]);
		exit(1);
	}

	original_record_file = argv[2];
	new_record_file = argv[3];
	if(!strcmp(argv[1], "-as")) {
		if ((inputfp = fopen(original_record_file, "r")) == NULL) {
			fprintf(stderr, "open error for %s\n", original_record_file);
			exit(1);
		}

		readHeader(inputfp, headerbuf);
		num_of_records = ((headerbuf[0] << 8) & 0xFF00) + (headerbuf[1] & 0xFF);
		num_of_inputs = num_of_records / RECORDS_NUM_INPUTBUF;
		if (num_of_records % RECORDS_NUM_INPUTBUF) 
			num_of_inputs++;

		for(i = 0; i < num_of_inputs; i++) {
			fseek(inputfp, HEADER_SIZE + i * INPUT_BUF_SIZE, SEEK_SET);
			makeRuns(inputfp, inputbuf);
		}

		fclose(inputfp);
/*
		if((outputfp = fopen(new_record_file, "w")) == NULL) {
			fprintf(stderr, "open error for %s\n", new_record_file);
			exit(1);
		}
		
		kwaymerge(outputfp, inputbuf, outputbuf);
		fclose(outputfp);*/
	}
	removeAllRuns();
	return 1;
}

void readHeader(FILE *fp, char *headerbuf) {
	fseek(fp, 0, SEEK_SET);
	fread(headerbuf, sizeof(char), HEADER_SIZE, fp);
}

void makeRuns(FILE *inputfp, char *inputbuf)
{
	char run_file[RUN_NAME_SIZE];
	FILE *runfp;
	int record_count = 0;
	int i;

	memset(inputbuf, 0x00, INPUT_BUF_SIZE);
	fread(inputbuf, sizeof(char), INPUT_BUF_SIZE, inputfp);	

#ifdef PRINT_FOR_DEBUG
	print_inputbuf(inputbuf);
#endif

	memset(run_file, 0x00, RUN_NAME_SIZE);
	snprintf(run_file, RUN_NAME_SIZE, "run%d.dat", run_count++);
	
	if((runfp = fopen(run_file, "w+")) == NULL) {
		fprintf(stderr, "open error for %s\n", run_file);
		exit(1);
	}
	
	for(i = 0; i < INPUT_BUF_SIZE; i += RECORD_SIZE) {
		if(inputbuf[i] == 0)
			break;
		record_count++;	
	}

	internalsort(inputbuf, record_count);

	fseek(runfp, 0, SEEK_SET);
	fwrite(inputbuf, sizeof(char), record_count * RECORD_SIZE, runfp);

	fclose(runfp);
}

void parse_inputbuf(char *inputbuf, STUDENT *s, int n) {
	int i, j;
	char recordbuf[RECORD_SIZE];
	
	for(i = 0; i < n; i++) {
		memset(recordbuf, 0x00, RECORD_SIZE);
		for(j = 0; j < RECORD_SIZE; j++)
			recordbuf[j] = inputbuf[j + i * RECORD_SIZE];
		unpack(recordbuf, &s[i]);
	}
}

void merge_inputbuf(char *inputbuf, STUDENT *s, int n) {
	int i, j;
	char recordbuf[RECORD_SIZE];

	memset(inputbuf, 0x00, INPUT_BUF_SIZE);
	for(i = 0; i < n; i++) {
		memset(recordbuf, 0x00, RECORD_SIZE);
		pack(recordbuf, &s[i]);
		for(j = 0; j < RECORD_SIZE; j++)
			inputbuf[i * RECORD_SIZE + j] = recordbuf[j];
	}
}

int compare_keyval(const void *st1, const void *st2) {
	int key1 = atoi(((STUDENT *)st1)->id);
	int key2 = atoi(((STUDENT *)st2)->id);

	if(key1 > key2)
		return 1;
	else if(key1 == key2)
		return 0;
	return -1;
}

void internalsort(char *inputbuf, int n)
{
	STUDENT st[n];
	parse_inputbuf(inputbuf, st, n);

#ifdef PRINT_FOR_DEBUG
	printf("---- Before ----\n");
	print_records(st, n);
#endif

	qsort(st, n, sizeof(STUDENT), compare_keyval);

#ifdef PRINT_FOR_DEBUG
	printf("---- After ----\n");
	print_records(st, n);
#endif
	merge_inputbuf(inputbuf, st, n);
#ifdef PRINT_FOR_DEBUG
	print_inputbuf(inputbuf); 	
#endif
}

void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf)
{

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

void unpack(const char *recordbuf, STUDENT *s)
{
	char tmpbuf[RECORD_SIZE];
	char *token;
	
	memset(tmpbuf, 0x00, RECORD_SIZE);
	memset(s, 0x00, sizeof(STUDENT));
	memcpy(tmpbuf, recordbuf, RECORD_SIZE);

	token = strtok(tmpbuf, "#");
	memcpy(s->id, token, ID_SIZE);

	token = strtok(NULL, "#");
	memcpy(s->name, token, NAME_SIZE);

	token = strtok(NULL, "#");
	memcpy(s->dept, token, DEPT_SIZE);
	
	token = strtok(NULL, "#");
	memcpy(s->year, token, YEAR_SIZE);

	token = strtok(NULL, "#");
	memcpy(s->addr, token, ADDR_SIZE);

	token = strtok(NULL, "#");
	memcpy(s->phone, token, PHONE_SIZE);

	token = strtok(NULL, "#");
	memcpy(s->email, token, EMAIL_SIZE);
}

void readChunk(FILE *runfp, char *inputbuf, int chunkid)
{
	int records_per_chunk = RECORDS_NUM_INPUTBUF / run_count;
	int input_index = chunkid * records_per_chunk * RECORD_SIZE;
	int chunk_size = records_per_chunk * RECORD_SIZE;
	fread(&inputbuf[input_index], sizeof(char), chunk_size, runfp);
}

void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n)
{
			
}

void removeAllRuns() {
	int i;
	char run_file[RUN_NAME_SIZE];

	for (i = 0; i < run_count; i++) {
		snprintf(run_file, RUN_NAME_SIZE, "run%d.dat", i);
		if (unlink(run_file) < 0) {
			fprintf(stderr, "unlink error for %s\n", run_file);
			exit(1);
		}
	}
}
void print_inputbuf(char *inputbuf) {
	int i;
	for(i = 0; i < INPUT_BUF_SIZE; i++)
		printf("%c", inputbuf[i]);
	printf("\n");
}

void print_records(STUDENT *s, int n) {
	int i;
	for(i = 0; i < n; i++) {
		printf(">> STUDENT %d <<\n", i);
		printf("ID = %s\n", s[i].id);
		printf("NAME = %s\n", s[i].name);
		printf("DEPT = %s\n", s[i].dept);
		printf("YEAR = %s\n", s[i].year);
		printf("ADDR = %s\n", s[i].addr);
		printf("PHONE = %s\n", s[i].phone);
		printf("EMAIL = %s\n", s[i].email);
		printf(">>END %d <<\n\n", i);
	}
}
