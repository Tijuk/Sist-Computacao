#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
A
--------B
--------C
--------D
----------------E
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
D = 4 ===> [4,1]
E = 5 ===> [5,8]
F = 6 ===> [6,0]
G = 7 ===> [7,0]
H = 8 ===> [8,11]
I = 9 ===> [9,0]
J = 10 ===> [10,0]
K = 11 ===> [11,0]
L = 12 ===> [12,15]
M = 13 ===> [13,16]
*/

typedef struct algumnome{
	int a;
	int b;
}A;

char* get_string(void);
char *getcode(char* s);
char *getfilenames(char *s);
char *fstspos(char *s);

int main(void)
{
	char *code;
	char *fst;
	code = get_string();
	//printf("Code: %s\n",code);
	fst = fstspos(code);
	printf("Fst: %s\n", fst);
	return 0;
}

char* get_string(void)
{
	char *s;
	s = (char*)malloc(sizeof(char)* 3000);
	char cod[] = "DL-REP";
	char allfilenames[] = "ABCDEFGHIJKLMN";
	char fstlspos[] = "1,0,2,1,3,1,4,1,5,8,6,0,7,0,8,11,9,0,10,0,11,0,12,15,13,16";
	sprintf(s,"%s,%s,%s",cod,allfilenames,fstlspos);
	return s;
}

char* fstspos(char *s)
{
	char *fsts;
	int i,j,n,tam;
	for(i=0,j=0; j < 2; i++) if(s[i] == ',') j++;
	n = i;
	tam = strlen(s) - (n-2);
	fsts = (char*)malloc(tam);
	for(i=0,j = n; s[j]!='\0';i++,j++)
	{
		fsts[i] = s[j];
	}
	return fsts;
}
