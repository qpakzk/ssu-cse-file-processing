#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ID_SIZE 8
#define NAME_SIZE 20
#define DEPT_SIZE 20
#define YEAR_SIZE 1
#define ADDR_SIZE 30
#define PHONE_SIZE 15
#define EMAIL_SIZE 26
#define RECORD_SIZE 127
#define HEADER_SIZE 4

#define NUM_NAME 100
#define NUM_DEPT 3
#define NUM_ADDR 50
#define SHUFFLE_NUM 10000

typedef struct _STUDENT {
	char id[ID_SIZE + 1];
	char name[NAME_SIZE + 1];
	char dept[DEPT_SIZE + 1];
	char year[YEAR_SIZE + 1];
	char addr[ADDR_SIZE + 1];
	char phone[PHONE_SIZE + 1];
	char email[EMAIL_SIZE + 1];
} STUDENT;

char *name_list[NUM_NAME] = {
	"Emma", "Liam", "Olivia", "Noah", "Ava", "Oliver", "Isabella", "Logan", "Sophia", "Mason", "Amelia", "Lucas", "Mia", "Ethan", "Charlotte", "Elijah", "Harper", "Aiden", "Mila", "James", "Aria", "Carter", "Ella", "Sebastian", "Evelyn", "Alexander", "Avery", "Jackson", "Abigail", "Michael", "Emily", "Jacob", "Riley", "Benjamin", "Luna", "Daniel", "Chloe", "William", "Scarlett", "Owen", "Layla", "Luke", "Sofia", "Jack", "Lily", "Grayson", "Ellie", "Henry", "Zoey", "Wyatt", "Madison", "Jayden", "Elizabeth", "Gabriel", "Grace", "Julian", "Penelope", "Matthew", "Victoria", "Leo", "Aubrey", "David", "Nora", "Jaxon", "Bella", "Levi", "Hannah", "Mateo", "Camila", "Samuel", "Aurora", "Muhammad", "Paisley", "John", "Addison", "Lincoln", "Stella", "Asher", "Savannah", "Isaac", "Natalie", "Isaiah", "Skylar", "Ryan", "Maya", "Adam", "Emilia", "Nathan", "Elena", "Caleb", "Hazel", "Joseph", "Violet", "Hunter", "Nova", "Anthony", "Niamey", "Josiah", "Eva", "Eli"
};

char *dept_list[NUM_DEPT] = {
	"Computer Science", "Engineering", "Science"
};

char *addr_list[NUM_ADDR] = {
"Alabama", "Alaska", "Arizona", "Arkansas", "California", "Colorado", "Connecticut", "Delaware", "Florida", "Georgia", "Hawaii", "Idaho", "Illinois", "Indiana", "Iowa", "Kansas", "Kentucky", "Louisiana", "Maine", "Maryland", "Massachusetts", "Michigan", "Minnesota", "Mississippi", "Missouri", "Montana", "Nebraska", "Nevada", "New Hampshire", "New Jersey", "New Mexico", "New York", "North Carolina", "North Dakota", "Ohio", "Oklahoma", "Oregon", "Pennsylvania", "Rhode Island", "South Carolina", "South Dakota", "Tennessee", "Texas", "Utah", "Vermont", "Virginia", "Washington", "West Virginia", "Wisconsin", "Wyoming"
};

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

void swap(STUDENT **st1, STUDENT **st2) {
	STUDENT *tmp = *st1;
	*st1 = *st2;
	*st2 = tmp;
}

int main(int argc, char *argv[]) {
	char *filename;
	int num_of_records;
	STUDENT **st;
	int i;
	int idx;
	char recordbuf[RECORD_SIZE];
	char headerbuf[HEADER_SIZE];
	int j, k;
	FILE *fp;

	srand(time(NULL));
	if(argc < 3) {
		fprintf(stderr, "USAGE: %s [NUM_OF_RECORDS] [RECORD_FILE]\n", argv[0]);
		exit(1);
	}
	num_of_records = atoi(argv[1]);
	filename = argv[2];
	st = (STUDENT **) malloc(num_of_records * sizeof(STUDENT *));

	for(i = 0; i < num_of_records; i++)
		st[i] = (STUDENT *)malloc(sizeof(STUDENT));

	for(i = 0; i < num_of_records; i++) {
		memset(st[i], 0x00, sizeof(STUDENT));
		snprintf(st[i]->id, ID_SIZE + 1, "%d", i + 1);
		idx = random() % NUM_NAME;
		snprintf(st[i]->name, NAME_SIZE + 1, "%s", name_list[idx]);
		snprintf(st[i]->dept, DEPT_SIZE + 1, "%s", dept_list[random() % NUM_DEPT]);
		snprintf(st[i]->year, YEAR_SIZE + 1, "%ld", random() % 4 + 1);
		snprintf(st[i]->addr, ADDR_SIZE + 1, "%s", addr_list[random() % NUM_ADDR]);
		snprintf(st[i]->phone, PHONE_SIZE + 1, "010-%ld%ld%ld%ld-%ld%ld%ld%ld", random() % 10, random() % 10, random() % 10, random() % 10, random() % 10, random() % 10, random() % 10, random() % 10);
		snprintf(st[i]->email, EMAIL_SIZE + 1, "%s@mail.com", name_list[idx]);
	}
	
	for(i = 0; i < SHUFFLE_NUM; i++) {
		j = random() % num_of_records;
		k = random() % num_of_records;
		swap(&st[j], &st[k]);
	}

	if((fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}	
	
	headerbuf[0] = (char) ((num_of_records & 0xFF00) >> 8);
	headerbuf[1] = (char) (num_of_records & 0xFF); 

	headerbuf[2] = (char) ((-1 &0xFF00) >> 8);
	headerbuf[3] = (char) (-1 & 0xFF);
	fseek(fp, 0, SEEK_SET);
	fwrite(headerbuf, 1, HEADER_SIZE, fp);
	for(i = 0; i < num_of_records; i++) {
		pack(recordbuf, st[i]);
		fseek(fp, HEADER_SIZE + i * RECORD_SIZE, SEEK_SET);
		fwrite(recordbuf, 1, RECORD_SIZE, fp);
	}

	fclose(fp);
	for(i = 0; i < num_of_records; i++)
		free(st[i]);
	free(st);
	exit(0);
}
