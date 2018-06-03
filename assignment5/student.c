#include <stdio.h>
#include "student.h"

void makeRuns(FILE *inputfp, const char *inputbuf);
void internalsort(const char *inputbuf, int n);
void kwaymerge(FILE *outputfp, const char *inputbuf, const char *outputbuf);
void pack(char *recordbuf, const STUDENT *s);
void unpack(const char *recordbuf, STUDENT *s);
void readChunk(FILE *runfp, const char *inputbuf, int chunkid);
void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n);

int main(int argc, char *argv[])
{
	FILE *inputfp, *outputfp;
	char inputbuf[INPUT_BUF_SIZE];
	char outputbuf[OUTPUT_BUF_SIZE];
    

	return 1;
}

void makeRuns(FILE *inputfp, const char *inputbuf)
{

}

void internalsort(const char *inputbuf, int n)
{

}

void kwaymerge(FILE *outputfp, const char *inputbuf, const char *outputbuf)
{

}

void pack(char *recordbuf, const STUDENT *s)
{

}
void unpack(const char *recordbuf, STUDENT *s)
{

}

void readChunk(FILE *runfp, const char *inputbuf, int chunkid);
{

}

void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n)
{

}
