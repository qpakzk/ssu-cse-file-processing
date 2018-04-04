#ifndef _RECORD_H_
#define _RECORD_H_

typedef struct _Student
{
	char id[11];
	char name[21];
	char address[51];
	char univ[21];
	char dept[31];
	char others[71];
} Student;

typedef struct _Record {
    char id[10];
    char name[20];
    char addr[50];
    char univ[20];
    char dept[30];
    char others[70];
    
} Record;
#endif
