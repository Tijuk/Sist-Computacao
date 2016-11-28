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

#define MP 100
#define MAXARG 10

typedef struct array_names{
	int a;
	int b;
}Ar;

typedef struct args_list{
	char arg[MAXARG][MP];
	int nargs;
}Args;

typedef struct info{
	char *code;
	char *path;
	int pathlen;
	char *path2;
	int path2len;
	char *payload;
	int nrbytes;
	int offset;
	int owner;
	char *permissions;
	Ar lst[40];
}Info;
char* scpy(char *s2);
void exibe_code(Info *a);
int check_code(char *code);
void get_args(char *s, Args *ar);
int read_code(char *s, Info *a, Args *ar);
void reader(int mode, Args *ar, Info *a);

int main(void)
{
	Info *a;
	Args *ar;
	char s[200];
	ar = (Args*)malloc(sizeof(Args));
	a = (Info*)malloc(sizeof(Info));
	//strcpy(s,"DC-REQ,path,strlen,dirname,strlen");
	strcpy(s,"RD-REQ,path,strlen,payload,nrbytes,offset");
	get_args(s,ar);
	read_code(s,a,ar);
	exibe_code(a);
	return 0;
}

void get_args(char *s, Args *ar)
{
	int i,j,k;
	j=k=0;
	for(i=0; i < strlen(s); i++)
	{
		if(s[i] == ',')
		{
			ar->arg[j][k] = '\0';
			j++;
			k = 0;
		}
		else
		{
			ar->arg[j][k] = s[i];
			k++;
		}
	}
	j++;
	ar->nargs = j;
}

int read_code(char *s, Info *a, Args *ar)
{
	int i;
	int code = check_code(ar->arg[0]);
	a->code = scpy(ar->arg[0]);
	if(code < 12)
	{
		reader(0,ar,a);
	}
	if(code >= 0 || code < 4)
	{
		reader(1,ar,a);
	}
	else if(code == 4)
	{
		reader(3,ar,a);
	}
	else if(code == 5 || code == 6)
	{
		reader(2,ar,a);
	}
	else if(code == 7)
	{
		
	}
}
void reader(int mode, Args *ar, Info *a)
{
	switch(mode)
	{
		case 0:
		{
			a->path = scpy(ar->arg[1]);
			a->pathlen = strlen(a->path);//atoi(ar->arg[2]);
			break;
		}
		case 1:
		{
			a->payload = scpy(ar->arg[3]);
			a->nrbytes = 0;//atoi(ar->arg[3]);
			a->offset = 0;//atoi(ar->arg[4]);
			break;
		}
		case 2:
		{
			a->path2 = scpy(ar->arg[3]);
			a->path2len = 0; //atoi(ar->arg[4]);
		}
		case 3:
		{
			a->owner = 0; //atoi(ar->arg[3]);
			a->permissions = scpy(ar->arg[4]);
			a->filelength = 0;// atoi(ar->arg[5]);
		}
	}
}

void exibe_code(Info *a)
{
	int code;
	code = check_code(a->code);
	if(code >= 0 || code < 4)
	{
		printf("Code: %s\nPath: %s\nStrlen: %d\nPayload: %s\nnrbytes: %d\nOffset: %d\n", a->code, a->path, a->pathlen, a->payload, a->nrbytes, a->offset);
	}
}

int check_code(char *code)
{
	int i;
	char codelist[12][6];
	strcpy(codelist[0],"RD-REQ");
	strcpy(codelist[1],"RD-REP");
	strcpy(codelist[2],"WR-REQ");
	strcpy(codelist[3],"WR-REP");
	strcpy(codelist[8],"FI-REQ");
	strcpy(codelist[4],"FI-REP");
	strcpy(codelist[5],"DC-REQ");
	strcpy(codelist[9],"DC-REP");
	strcpy(codelist[6],"DR-REQ");
	strcpy(codelist[10],"DR-REP");
	strcpy(codelist[11],"DL-REQ");
	strcpy(codelist[7],"DL-REP");
	for(i=0; i < 12; i++)
	{
		if(strcmp(code,codelist[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

char* scpy(char *s2)
{
	int n,i;
	char *s1;
	n = strlen(s2) + 1;
	s1 = (char*)malloc(n * sizeof(char));
	strcpy(s1,s2);
	return s1;
}
