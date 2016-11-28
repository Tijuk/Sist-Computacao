/*
	opcoes possiveis:
	RD-REQ, path, strlen, payload, nrbytes, offset
	RD-REP, path, strlen, payload, nrbytes, offset
	WR-REQ, path, strlen, payload, nrbytes, offset
	WR-REP, path, strlen, payload, nrbytes, offset

	FI-REQ, path, strlen
	FI-REP, path, strlen, owner, permissions, filelength

	DC-REQ, path, strlen, dirname, strlen
	DC-REP, path, strlen
	DR-REQ, path, strlen, dirname, strlen
	DR-REP, path, strlen
	DL-REQ, path, strlen
	DL-REP, allfilenames, fstlstpositions(array[40] of struct {int, int}, nrnames
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct array_names{
	int a;
	int b;
}Ar;

void findallcommas(char *s, int *comma, int *n);
int check_code(char *code);
int reader(char *s, char *code, char *path, int strlen);

void read_1(char *s, char *code, char *path, int strlen, char *payload, int nrbytes, int offset)
{
	
}
int reader(char *s, char *code, char *path, int strlen)
{

}

int main(void)
{
	int i,j,k;
	int *comma, ncomma;
	char s[200];
	char ar[10][100];
	//printf("Decision:\n");
	//scanf(" %s", s);
	strcpy(s,"DC-REQ,path,strlen,dirname,strlen");
	findallcommas(s,comma,&ncomma);
	printf("ok\n");
	j=k=0;
	for(i=0; i < strlen(s); i++)
	{
		printf("ok %d\n",i);
		ar[j][k] = s[i];
		k++;
		printf("ok\n");
		if(i == comma[j])
		{
			j++;
			k = 0;
		}
		printf("ok\n");
	}
	for(i=0; i < ncomma; i++)
	{
		printf("Arg: %d - %s\n", i, ar[i]);
	}
	return 0;
}
	
	

int check_code(char *code)
{
	int i;
	char codelist[12][6];
	strcpy(codelist[0],"RD-REQ");
	strcpy(codelist[1],"RD-REP");
	strcpy(codelist[2],"WR-REQ");
	strcpy(codelist[3],"WR-REP");
	strcpy(codelist[4],"FI-REQ");
	strcpy(codelist[5],"FI-REP");
	strcpy(codelist[6],"DC-REQ");
	strcpy(codelist[7],"DC-REP");
	strcpy(codelist[8],"DR-REQ");
	strcpy(codelist[9],"DR-REP");
	strcpy(codelist[10],"DL-REQ");
	strcpy(codelist[11],"DL-REP");
	for(i=0; i < 12; i++)
	{
		if(strcmp(code,codelist[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

void findallcommas(char *s, int *comma, int *n)
{
	int i, nc;
	for(i=0, nc=0; i< strlen(s); i++)
	{
		if(s[i] == ',') nc++;
	}
	comma = (int*)malloc(nc*sizeof(int));
	for(i=0, nc = 0; i < strlen(s);i++)
	{
		if(s[i] == ',')
		{
			comma[nc] = i;
			nc++;
		}
	}
}









