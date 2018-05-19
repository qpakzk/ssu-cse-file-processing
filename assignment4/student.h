#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_SIZE		127	//id(8) + name(20) + department(20) + year(1) + address(30) + phone(15) +  email(26) + delimeters(7)
#define HEADER_SIZE		4	//#records(2) + head(2)

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

#endif
