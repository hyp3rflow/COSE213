#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_YEAR_DURATION 10 // 기간

// 구조체 선언
typedef struct
{
	char name[20];				 // 이름
	char sex;					 // 성별 M or F
	int freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct
{
	int len;	  // 배열에 저장된 이름의 수
	int capacity; // 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName *data;  // 이름 배열의 포인터
} tNames;

// 함수 원형 선언

// 연도별 입력 파일을 읽어 이름 정보(이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// names->capacity는 2배씩 증가
void load_names(FILE *fp, int year_index, tNames *names)
{
	char str[30];

	while (fgets(str, 30, fp) != NULL)
	{
		char name[20];
		char sex;
		int frequency;

		char *ptr = strtok(str, ",");
		sprintf(name, "%s", ptr);
		fprintf(stderr, ">> %s\n", name);

		ptr = strtok(NULL, ",");
		sex = ptr[0];

		ptr = strtok(NULL, ",");
		frequency = atoi(ptr);

		int flag = 0;
		for (int i = 0; i < names->len; i++)
		{
			tName curr = (names->data)[i];

			if (curr.name == name && curr.sex == sex)
			{
				flag++;
				curr.freq[year_index] += frequency;
				break;
			}
		}

		if (!flag)
		{
			if (names->capacity == names->len)
			{
				names->capacity *= 2;
				fprintf(stderr, "realloc start!\n");
				names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));
			}
			tName newNode = (names->data)[names->len];
			strcpy(newNode.name, name);
			newNode.sex = sex;
			newNode.freq[year_index] = frequency;

			names->len++;
		}
	}
}

// 구조체 배열을 화면에 출력
void print_names(tNames *names, int num_year)
{
	for (int i = 0; i < names->len; i++)
	{
		tName curr = (names->data)[i];
		printf("%s", curr.name);
		printf("\t%c", curr.sex);
		for (int j = 0; j < num_year; j++)
		{
			printf("\t%d", curr.freq[j]);
		}
		printf("\n");
	}
}

// qsort를 위한 비교 함수
int compare(const void *n1, const void *n2)
{
	tName fst = *(tName *)n1;
	tName snd = *(tName *)n2;

	return !strcmp(fst.name, snd.name) ? fst.sex < snd.sex : strcmp(fst.name, snd.name);
}

// 함수 정의

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc(sizeof(tNames));

	pnames->len = 0;
	pnames->capacity = 1;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;

	int num = 0;
	FILE *fp;
	int num_year = 0;

	if (argc == 1)
		return 0;

	// 이름 구조체 초기화
	names = create_names();

	fprintf(stderr, "name initialized!\n");

	// 첫 연도 알아내기 "yob2009.txt" -> 2009
	int start_year = atoi(&argv[1][3]);

	for (int i = 1; i < argc; i++)
	{
		num_year++;
		fp = fopen(argv[i], "r");
		assert(fp != NULL);

		int year = atoi(&argv[i][3]); // ex) "yob2009.txt" -> 2009

		fprintf(stderr, "Processing [%s]..\n", argv[i]);

		// 연도별 입력 파일(이름 정보)을 구조체에 저장
		load_names(fp, year - start_year, names);

		fclose(fp);
	}

	// 정렬 (이름순 (이름이 같은 경우 성별순))
	qsort(names->data, names->len, sizeof(tName), compare);

	// 이름 구조체를 화면에 출력
	print_names(names, num_year);

	// 이름 구조체 해제
	destroy_names(names);

	return 1;
}
