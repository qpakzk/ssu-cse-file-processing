#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_SIZE 127
#define HEADER_SIZE	4
#define ID_SIZE 8
#define NAME_SIZE 20
#define DEPT_SIZE 20
#define YEAR_SIZE 1
#define ADDR_SIZE 30
#define PHONE_SIZE 15
#define EMAIL_SIZE 26

typedef struct _STUDENT
{
	char id[9];
	char name[21];
	char dept[21];
	char year[2];
	char addr[31];
	char phone[16];
	char email[27];
} STUDENT;

#define RECORDS_NUM_INPUTBUF	100
#define RECORDS_NUM_OUTPUTBUF 	10
#define INPUT_BUF_SIZE	(RECORDS_NUM_INPUTBUF * RECORD_SIZE)
#define OUTPUT_BUF_SIZE	(RECORDS_NUM_OUTPUTBUF * RECORD_SIZE)

#endif
