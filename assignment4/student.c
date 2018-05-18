#include <stdio.h>
#include "student.h"

void pack(char *recordbuf, const STUDENT *s);
void unpack(const char *recordbuf, STUDENT *s);
void read(FILE *fp, char *recordbuf, int rrn);
void add(FILE *fp, const STUDET *s);
int search(FILE *fp, const char *keyval);
void delete(FILE *fp, const char *keyval);
void printRecord(const STUDENT *s, int n);

int main(int argc, char *argv[])
{
	FILE *fp;


	return 1;
}

void printRecord(const STUDENT *s, int n)
{
	int i;

	for(i=0; i<n; i++)
	{
		printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}
