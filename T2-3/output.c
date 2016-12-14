#include "output.h"

#define OK 0
#define NBGREATER 1
#define STRGREATER 2

#define CREATED 0
#define WR_RD 1
#define DELETED 2
#define OTHER 3

static int status;
static int need_decode = 0;

void remove_state(char *output);
void rem_barra(char *s);
char* get_nword(int word_number, char *anyput);
int get_mode_output(char *s);
void erro_de_permissao(void);
void failure(int mode, char* path);
void message(int mode, int state,char* output);
void sucess_message(int stat, int mode, char* output);

int check_decode(void)
{
	return need_decode;
}

void sucess_message(int stat, int mode, char* output)
{
	char *path;
	char *w;
	path = get_nword(3,output);
	rem_barra(path);
	if(mode == DEF_RD || mode == DEF_WR)
	{
		int nb;
		w = get_nword(6,output);
		if(mode == DEF_RD)
		{
			printf("\tYou read < %d > bytes from < %s > file\n",nb,path);
			printf("\tYou read the string < %s >\n", get_nword(5,output));
		}
		else if(mode == DEF_WR)
		{
			if(stat == WROTE)
			{
				printf("\tYou wrote < %d > bytes from < %s > file\n",nb,path);
			}
			else if(stat == CREATED)
			{
				printf("\tYou created the file < %s >\n",path);
			}
			else if(stat == REMOVED)
			{
				printf("\tYou deleted the file < %s >\n",path);
			}
		}
	}
	else if(mode == DEF_FI)
	{
		int owner, filelength;
		char *perm; 
		w = get_nword(5,output);
		owner = atoi(w);
		w = get_nword(7,output);
		filelength = atoi(w);
		perm = get_nword(6,output);
		printf("\tYou accessed the file < %s >\n", path);
		printf("\t\tOwnerID: %d\n\t\tWriting Permission: %c\n\t\tReading Permission: %c\n",owner,perm[0],perm[1]);
		printf("\t\tFilelength: %d\n", filelength);
	}
	else if(mode == DEF_DC)
	{
		printf("Directory < %s > was created\n", path);
	}
	else if(mode == DEF_DR)
	{
		printf("Directory < %s > was removed\n", path);
	}
	else if(mode == DEF_DL)
	{
		need_decode = 1;
		remove_state(output);
	}
	else if(mode == DEF_OL)
	{
		need_decode = 1;
		remove_state(output);
	}
	else if(mode == DEF_CL)
	{
		printf("File < %s > had its permissions changed\n", path);
	}
}

void exibe_output(char *output)
{
	int mode;
	int state;
	char *word1 = get_nword(1, output);
	char *word2 = get_nword(2, output);
	state = atoi(word1);
	mode = get_mode_output(word2);
	printf("%s\n",word2);
	message(mode, state, output);
}


char* get_nword(int word_number, char *output)
{
	int i, j, k,m;
	char *action;
	char word[MAX_SIZE];
	m = strlen(output) + 1;
	for (i = 0, j = 0, k = 0; i < m; i++, k++)
	{
		word[k] = output[i];
		if (output[i] == ',' || output[i] == '\0')
		{
			word[k] = '\0';
			j++;
			k = -1;
			if (word_number == j) break;
		}
	}
	action = (char*)malloc(strlen(word)+1 * sizeof(char));
	strcpy(action, word);
	return action;
}

void message(int mode, int state,char* output)
{
	printf("mode\t%s\n",output);
	if(state >= CREATED)
	{
		printf("state %d\tmode %d\n",state,mode);
		sucess_message(state,mode, output);
	}
	else
	{
		char *path = get_nword(3,output);
		failure(mode,path);
		if(state == INSUF_PERM)
		{
			erro_de_permissao();
		}
		else if(state == FILE_ALREADY_EXISTS)
		{
			printf("\tFile already exists.\n");
		}
	}
}

int get_mode_output(char *s)
{
	int val;
	if (strcmp(s, "RD-REP") == 0) val = 0;
	else if (strcmp(s, "WR-REP") == 0) val = 1;
	else if (strcmp(s, "FI-REP") == 0) val = 2;
	else if (strcmp(s, "DC-REP") == 0) val = 3;
	else if (strcmp(s, "DR-REP") == 0) val = 4;
	else if (strcmp(s, "DL-REP") == 0) val = 5;
	else if (strcmp(s, "OL-REP") == 0) val = 6;
	else if (strcmp(s, "CL-REP") == 0) val = 7;
	else val = -1;
	return val;
}

void erro_de_permissao(void)
{
	printf("\tError. Action could not be completed due to lack of permissions\n");
	printf("\tTry using [shinfo] for more information and [chperm] to fix the problem\n");
	printf("\tFor more information on [shinfo] and [chperm] use [man]\n");
}

void failure(int mode, char* path)
{
	printf("\t");
	if(mode == DEF_RD)
	{
		printf("Failed to read file < %s >", path);
	}
	else if(mode == DEF_WR)
	{
		printf("Failed to write on file < %s >", path);
	}
	else if(mode == DEF_FI)
	{
		printf("Failed to look for information from file < %s >", path);
	}
	else if(mode == DEF_DC)
	{
		printf("Failed to create directory < %s >", path);
	}
	else if(mode == DEF_DR)
	{
		printf("Failed to delete directory < %s >", path);
	}
	else if(mode == DEF_DL)
	{
		printf("Failed to list all directories and subdirectories from path < %s >", path);
	}
	else if(mode == DEF_OL)
	{
		printf("Failed to list all directories from path < %s >", path);
	}
	else if(mode == DEF_CL)
	{
		printf("Failed to change permissions from path < %s >", path);
	}
	printf("\tCheck [help] and [man] for more information\n");
}

void rem_barra(char *s)
{
	int i;
	for(i=0; i < strlen(s); i++)
	{
		s[i] = s[i+1];
	}
}

void remove_state(char *output)
{
	int i,j;
	for(i=0; output[i] != ','; i++);i++;
	for(j=0; j < strlen(output) - i; j++)
	{
		output[j] = output[j+i];
	}
}
