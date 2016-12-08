#include "code_decode.h"
#define CMAX 3000

/* ------------------------------------------------- *\
		ENCODE
\* ------------------------------------------------- */

typedef struct lista_wut{
	char path[40][300];
	int psize[40];
	int n;
}ENC;

ENC* encoder;

void adjust_path(char *path);
char* isolate_file_name(char *path);
char* strcat_all_paths(void);
int count_bars(int n);
char* vet2string(int *a, int *b);

void start_code(void)
{
	encoder = (ENC*)malloc(sizeof(ENC));
	encoder->n = 0;
	return;
}
void ins_path(char *path)
{
	char *isolated;
	char *newpath;
	newpath = (char*)malloc(sizeof(char)* 300);
	strcpy(newpath, path);
	adjust_path(newpath);
	strcpy(encoder->path[encoder->n], newpath);
	isolated = isolate_file_name(newpath);
	encoder->psize[encoder->n] = strlen(isolated);
	encoder->n++;
}
char* isolate_file_name(char *path)
{
	char* file;
	int i, j;
	file = (char*)malloc(300 * sizeof(char));
	i = j = 0;
	for (i = strlen(path); path[i] != '/'; i--);
	i++;
	for (; path[i] != '\0'; i++,j++)
	{
		 file[j] = path[i];
	}
	file[j] = '\0';
	return file;
}
void adjust_path(char *path)
{
	int n = strlen(path);
	if (path[n - 1] == '/')
	{
		path[n - 1] = '\0';
	}
}
char* strcat_all_paths(void)
{
	int i;
	char *all;
	char *path;
	all = (char*)malloc(sizeof(char)*CMAX);
	path = (char*)malloc(sizeof(char)*300);
	strcpy(all, encoder->path[0]);
	for (i = 0; i < encoder->n; i++)
	{
		path = isolate_file_name(encoder->path[i]);
		if (i == 0)
		{
			strcpy(all, path);
		}
		else
		strcat(all, path);
	}
	return all;
}
char* pos(void);

char* encode(void)
{
	char* final;
	char* code = (char*)malloc(sizeof(char)*CMAX);
	char* po = pos();
	final = code;
	char pre[] = "DL-REQ";
	code = strcat_all_paths();
	sprintf(final, "%s,%s%s", pre, code,po);
	return final;
}
int count_bars(int n)
{
	int i, j;
	for (i = 0, j = 0; encoder->path[n][i] != '\0'; i++)
	{
		if (encoder->path[n][i] == '/')
		{
			j++;
		}
	}
	return j;
}
char* pos(void)
{
	int a[40];
	int b[40];
	int i,lvl;
	int spc = 1;
	int minlvl = count_bars(0);
	int lastlvl;
	a[0] = 1;
	b[0] = 0;
	lastlvl = lvl = minlvl;
	for (i = 1; i < encoder->n; i++)
	{
		spc += encoder->psize[i - 1];
		a[i] = spc;
		lvl = count_bars(i);
		if (lastlvl < lvl)
		{
			b[i] = b[i - 1] + 1;
		}
		else if (lastlvl == lvl) b[i] = b[i - 1];
		else b[i] = b[i - 1] - 1;
		if (lvl == minlvl) b[i] = 0;
		lastlvl = lvl;
	}
	return vet2string(a,b);
}
char* vet2string(int *a, int *b)
{
	char *s, num1[4], num2[4], *aux;
	int i;
	s = (char*)malloc(sizeof(char)*CMAX);
	aux = (char*)malloc(sizeof(char)*300);
	for (i = 0; i < encoder->n; i++)
	{
		sprintf(num1, "%d", a[i]);
		sprintf(num2, "%d", b[i]);
		sprintf(aux, ",%s,%s", num1, num2);
		if (i == 0) strcpy(s, aux);
		else strcat(s, aux);
	}
	return s;
}
/* ------------------------------------------------- *\
DECODE
\* ------------------------------------------------- */

/*
A
--------B
--------C
----------------D
--------E
F
G
--------H
I
J
K
--------L
----------------M
*/

/*
letra = posicao  ===> [posicao,nivel]
A = 1 ===> [1,0]
B = 2 ===> [2,1]
C = 3 ===> [3,1]
D = 4 ===> [4,2]
E = 5 ===> [5,1]
F = 6 ===> [6,0]
G = 7 ===> [7,0]
H = 8 ===> [8,1]
I = 9 ===> [9,0]
J = 10 ===> [10,0]
K = 11 ===> [11,0]
L = 12 ===> [12,1]
M = 13 ===> [13,2]
*/

typedef struct algumnome{
	int a[40];
	int b[40];
	int n_elem;
	char filenames[40][300];
}A;

A* get_struct(char *fst, char *filenms);
char *getfilenames(char *s);
char *fstspos(char *s);
void fill_ab(A* news, char *s);
int get_num(char *s, int n);
int get_n_num(char *s);
char* word(A* news, int n, char* code);
void exibe_decoded(A* decode);
void space(int n);

void decode(char *code)
{
	A* decoded;
	char *fst;
	char *flnm;
	fst = fstspos(code);
	flnm = getfilenames(code);
	decoded = get_struct(fst, flnm);
	exibe_decoded(decoded);
}
char* getfilenames(char *s)
{
	char* filenames;
	int i, j, k;
	filenames = (char*)malloc(3000 * sizeof(char));
	for (i = 0, j = 0, k = 0; k<2; i++)
	{
		if (s[i] == ',') k++;
		else if (k == 1)
		{
			filenames[j] = s[i];
			j++;
		}
	}
	filenames[j] = '\0';
	return filenames;
}
char* fstspos(char *s)
{
	char *fsts;
	int i, j, n, tam;
	for (i = 0, j = 0; j < 2; i++) if (s[i] == ',') j++;
	n = i;
	tam = strlen(s) - (n - 2);
	fsts = (char*)malloc(tam);
	for (i = 0, j = n; s[j] != '\0'; i++, j++)
	{
		fsts[i] = s[j];
	}
	fsts[i] = '\0';
	return fsts;
}
int get_num(char *s, int n)
{
	char number[4];
	int i, j, virgula;
	virgula = j = 0;
	for (i = 0; s[i] != '\0'; i++)
	{
		if (virgula == n)
		{
			number[j] = s[i];
			j++;
			if (j > 4)break;
		}
		if (s[i] == ',') virgula++;
	}
	number[j] = '\0';
	return atoi(number);
}
int get_n_num(char *s)
{
	int i, val;
	for (i = 0, val = 0; s[i] != '\0'; i++)
	{
		if (s[i] == ',')val++;
	}
	return val + 1;
}
void fill_ab(A* news, char *s)
{
	int i, j;
	news->n_elem = get_n_num(s) / 2;
	for (i = 0; i < news->n_elem; i++)
	{
		j = 2 * i;
		news->a[i] = get_num(s, j);
		news->b[i] = get_num(s, j + 1);
	}
}
char* word(A* news, int n, char* code)
{
	int i, j, k;
	int comeco, fim;
	char *word;
	comeco = news->a[n] - 1;
	fim = news->a[n + 1] - 1;
	i = j = k = 0;
	word = (char*)malloc(300 * sizeof(char));
	for (i = comeco; i < fim; i++)
	{
		word[i - comeco] = code[i];
	}
	word[i - comeco] = '\0';
	return word;
}
void get_all_word(A* news, char* s)
{
	int i;
	char *w;
	for (i = 0; i < news->n_elem - 1; i++)
	{
		w = word(news, i, s);
		strcpy(news->filenames[i], w);
	}
}
void space(int n)
{
	int i;
	if (n > 0){
		for (i = 0; i < n - 1; i++)printf("\t");
		printf(" \\-----> ");
	}
}
A* get_struct(char *fst, char *filenms)
{
	A* news = (A*)malloc(sizeof(A));
	fill_ab(news, fst);
	get_all_word(news, filenms);
	return news;
}
void exibe_decoded(A* decode)
{
	int i;
	int last = 0;
	int lvl = 0;
	int v;
	for (i = 0; i < decode->n_elem - 1; i++)
	{
		v = decode->b[i];
		/*if (v == 0)
		{
			lvl = 0;
		}
		else if (v != last)
		{
			lvl++;
		}*/
		lvl = v;
		space(lvl);
		last = v;
		printf("%s\n", decode->filenames[i]);
		//printf("%s - %d - %d\n", decode->filenames[i], decode->a[i], decode->b[i]);
	}
}
