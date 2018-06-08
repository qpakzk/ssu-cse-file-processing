//#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "student.h"

typedef struct _RUN {
	FILE *runfp;
	//for run
	int chunkid;
	int num_of_records_in_run;
	bool end_run;
	//for inputbuf
	int index;
	int num_of_records_in_buf;
	bool end_chunk;
} RUN;

void readHeader(FILE *fp, char *headerbuf);
void makeRuns(FILE *inputfp, char *inputbuf);
void parse_inputbuf(char *inputbuf, STUDENT *s, int n);
void merge_inputbuf(char *inputbuf, STUDENT *s, int n);
int compare_keyval(const void *st1, const void *st2);
void internalsort(char *inputbuf, int n);
int find_keyval(char *inputbuf, int index);
void initialize_output(FILE *outputfp);
void initialize_run(RUN *run_file);
void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf);
void pack(char *recordbuf, const STUDENT *s);
void unpack(const char *recordbuf, STUDENT *s);
void readChunk(FILE *runfp, char *inputbuf, int chunkid);
void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n);
void removeAllRuns();
void print_inputbuf(char *inputbuf);
void print_records(STUDENT *s, int n);

int run_count;
int chunk_size;

int main(int argc, char *argv[]) {
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

		if((outputfp = fopen(new_record_file, "r")) == NULL) {
			outputfp = fopen(new_record_file, "w");
		}

		fclose(outputfp);
		
		if((outputfp = fopen(new_record_file, "r+")) == NULL) {
			fprintf(stderr, "open error for %s\n", new_record_file);
			exit(1);
		}
		
		kwaymerge(outputfp, inputbuf, outputbuf);
		fclose(outputfp);
	}
	//removeAllRuns();
	return 1;
}

void readHeader(FILE *fp, char *headerbuf) {
	fseek(fp, 0, SEEK_SET);
	fread(headerbuf, sizeof(char), HEADER_SIZE, fp);
}

void makeRuns(FILE *inputfp, char *inputbuf) {
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

void internalsort(char *inputbuf, int n) {
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

int find_keyval(char *inputbuf, int index) {
	char recordbuf[RECORD_SIZE];
	char *token;
	int keyval;

	memcpy(recordbuf, inputbuf + index * RECORD_SIZE, RECORD_SIZE);
	token = strtok(recordbuf, "#");
	if(!token)
		return -1;
	keyval = atoi(token);
	return keyval;
}

void initialize_output(FILE *outputfp) {
	char headerbuf[HEADER_SIZE];
	short num_of_records = 0;
	short del_rrn = -1;

	memset(headerbuf, 0x00, HEADER_SIZE);
	headerbuf[0] = (char)((num_of_records & 0xFF00) >> 8);
	headerbuf[1] = (char)(num_of_records & 0xFF);
	headerbuf[2] = (char)((del_rrn & 0xFF00) >> 8);
	headerbuf[3] = (char)(del_rrn & 0xFF);

	fseek(outputfp, 0, SEEK_SET);
	fwrite(headerbuf, sizeof(char), HEADER_SIZE, outputfp);
}

void initialize_run(RUN *run_file) {
	int i;
	char run_name[RUN_NAME_SIZE];
	int file_size;
	int chunk_size;
	memset(run_file, 0x00, sizeof(RUN) * run_count);

	for (i = 0; i < run_count; i++) {
		memset(run_name, 0x00, RUN_NAME_SIZE);
		snprintf(run_name, RUN_NAME_SIZE, "run%d.dat", i);
		if ((run_file[i].runfp = fopen(run_name, "r")) == NULL) {
			fprintf(stderr, "open error for %s\n", run_name);
			exit(1);
		}
	}

	for(i = 0; i < run_count; i++) {
		fseek(run_file[i].runfp, 0, SEEK_END);
		file_size = ftell(run_file[i].runfp);
		run_file[i].num_of_records_in_run = file_size / RECORD_SIZE;
	}

	chunk_size = INPUT_BUF_SIZE / run_count;
	for(i = 0; i < run_count; i++)
		run_file[i].num_of_records_in_buf = chunk_size / RECORD_SIZE;
}

void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf) {
	int i;
	int run_num;
	RUN run_file[run_count];
	int lowest_run;
	int lowest_key, candidate_key;
	bool is_sorted;
	int output_index = 0;

	chunk_size = INPUT_BUF_SIZE / run_count;
	if (chunk_size < RECORD_SIZE) {
		fprintf(stderr, "Chunk size s smaller than record size.\n");
		exit(1);
	}
	initialize_run(run_file);

	memset(inputbuf, 0x00, INPUT_BUF_SIZE);
	memset(outputbuf, 0x00, OUTPUT_BUF_SIZE);
	initialize_output(outputfp);

	for (i = 0; i < run_count; i++) {
		readChunk(run_file[i].runfp, &inputbuf[i * chunk_size], run_file[i].chunkid++);
		if (run_file[lowest_run].chunkid * run_file[lowest_run].num_of_records_in_buf >= run_file[lowest_run].num_of_records_in_run)
			run_file[lowest_run].end_run = true;
	}

	while(1) {
		run_num = 0;
		while(inputbuf[run_num * chunk_size] == 0)
			run_num++;
		lowest_run = run_num;
		lowest_key = find_keyval(&inputbuf[lowest_run * chunk_size], run_file[lowest_run].index);
		for (i = 0; i < run_count; i++) {
			if (run_file[i].end_run || i == run_num || inputbuf[i * chunk_size] == 0)
				continue;
			candidate_key = find_keyval(&inputbuf[i * chunk_size], run_file[i].index);
			if(lowest_key > candidate_key) {
				lowest_key = candidate_key;
				lowest_run = i;
			}
		}
		for(i = 0; i < RECORD_SIZE; i++) {
			outputbuf[output_index] = inputbuf[lowest_run * chunk_size + run_file[lowest_run].index * RECORD_SIZE + i];
			output_index++;
		}

		if(output_index >= OUTPUT_BUF_SIZE) {
			printf("output_index = %d\n", output_index);
			printf("output >>\n");
			for(i = 0; i < OUTPUT_BUF_SIZE; i++) {
				if(i % RECORD_SIZE == 0)
					printf("\n");
				printf("%c", outputbuf[i]);
			}
			printf("\n");
			writeOutputbuf(outputfp, outputbuf, output_index);
			memset(outputbuf, 0x00, OUTPUT_BUF_SIZE);
			output_index = 0;
		}

		run_file[lowest_run].index++;
		if(run_file[lowest_run].index >= run_file[lowest_run].num_of_records_in_buf)
			run_file[lowest_run].end_chunk = true;

		if (run_file[lowest_run].end_chunk) {
			run_file[lowest_run].index = 0;
			run_file[lowest_run].end_chunk = false;
			if (run_file[lowest_run].chunkid * run_file[lowest_run].num_of_records_in_buf >= run_file[lowest_run].num_of_records_in_run) {
				run_file[lowest_run].end_run = true;
				for (i = lowest_run * chunk_size; i < chunk_size; i++)
					inputbuf[i] = 0;
				
				is_sorted = true;
				for (i = 0; i < run_count; i++) {
					if (run_file[i].end_run == false) {
						is_sorted = false;
						break;
					}
				}
				
				if (is_sorted) {
					memset(inputbuf, 0x00, INPUT_BUF_SIZE);
					writeOutputbuf(outputfp, outputbuf, output_index);
					memset(outputbuf, 0x00, OUTPUT_BUF_SIZE);
					output_index = 0;
					break;
				}
			}
			else {
				readChunk(run_file[lowest_run].runfp, &inputbuf[lowest_run * chunk_size], run_file[lowest_run].chunkid++);
				if (run_file[lowest_run].chunkid * run_file[lowest_run].num_of_records_in_buf >= run_file[lowest_run].num_of_records_in_run)
					run_file[lowest_run].end_run = true;
			}
		}
	}
	
	for (i = 0; i < run_count; i++)
		fclose(run_file[i].runfp);
}

void pack(char *recordbuf, const STUDENT *s) {
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

void unpack(const char *recordbuf, STUDENT *s) {
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

void readChunk(FILE *runfp, char *inputbuf, int chunkid) {
	int num_of_records;

	num_of_records = chunk_size / RECORD_SIZE;
	fseek(runfp, chunkid * num_of_records * RECORD_SIZE, SEEK_SET);
	fread(inputbuf, sizeof(char), num_of_records * RECORD_SIZE, runfp);
}

void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n) {
	char headerbuf[HEADER_SIZE];
	short num_of_records;
	short del_rrn;
	int i;

	memset(headerbuf, 0x00, HEADER_SIZE);
	fseek(outputfp, 0, SEEK_SET);
	fread(headerbuf, sizeof(char), HEADER_SIZE, outputfp);

	num_of_records = ((headerbuf[0] << 8) & 0xFF00) + (headerbuf[1] & 0xFF);
	del_rrn = ((headerbuf[2] << 8) & 0xFF00) + (headerbuf[3] & 0xFF);

	fseek(outputfp, HEADER_SIZE + num_of_records * RECORD_SIZE, SEEK_SET);
	fwrite(outputbuf, sizeof(char), n, outputfp);

	num_of_records += (n / RECORD_SIZE);

	memset(headerbuf, 0x00, HEADER_SIZE);
	headerbuf[0] = (char)((num_of_records & 0xFF00) >> 8);
	headerbuf[1] = (char)(num_of_records & 0xFF);
	headerbuf[2] = (char)((del_rrn & 0xFF00) >> 8);
	headerbuf[3] = (char)(del_rrn & 0xFF);

	fseek(outputfp, 0, SEEK_SET);
	fwrite(headerbuf, sizeof(char), HEADER_SIZE, outputfp);		
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
