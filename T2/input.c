#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_SIZE 300
#define MAX_SIZE 100
#define SPACE 32

/* mode:
	mkdir = 55
	rmdir = 62
*/

int get_mode(char *s);
void get_all_commands(int index1, int index2, char *command);
char* get_word(int word_number, char *cmd);
char* get_exec(char *path, char *input);

int main(void)
{
	char *execucao;
	char path[MAX_PATH_SIZE];
	char input[MAX_SIZE];
	strcpy(path,"SFS-root-di/");
	strcpy(input,"mkdir diretorio");
	execucao = get_exec(path, input);
	printf("Commando: %s\n",execucao);

	strcpy(input,"rmdir diretorio");
	execucao = get_exec(path, input);
	printf("\n\nCommando: %s\n",execucao);
	return 0;
}
char* get_exec(char *path, char *input)
{
	char *exec;
	char *action;
	char cmd[7];
	int mode;
	exec = (char*)malloc(MAX_PATH_SIZE * sizeof(char));
	action = get_word(1,input);
	mode = get_mode(action);
	printf("mode: %d\n",mode);
	if(mode == 0)
	{
		char *dirname;		
		get_all_commands(3,0,cmd);
		dirname = get_word(2, input);
		sprintf(exec, "%s,%s,%d,%s,%d",cmd,path,strlen(path),dirname, strlen(dirname));
		return exec;
	}
	else
	{
		printf("Input Invalido\n");
		return NULL;
	}
}
char* get_word(int word_number, char *input)
{
	int i,j,k;
	char *action;
	char word[MAX_SIZE];
	char words[10][MAX_SIZE];
	int nsize[10];
	for(i=0; i<10; i++) nsize[i] = 0;
	for(i=0,j=0,k=0;i < strlen(input)+1;i++,k++)
	{
		word[k] = input[i];
		if(input[i] == SPACE || input[i] == '\0')
		{
			word[k] = '\0';
			j++;
			k = -1;
			if(word_number == j) break;
		}
	}
	action = (char*)malloc(strlen(word) * sizeof(char));
	strcpy(action,word);
	return action;
}

void get_all_commands(int index1, int index2, char *command)
{
	char s[6][7];
	strcpy(s[0],"RD-");
	strcpy(s[1],"WR-");
	strcpy(s[2],"FI-");
	strcpy(s[3],"DC-");
	strcpy(s[4],"DR-");
	strcpy(s[5],"DL-");
	if(index2 == 0) strcat(s[index1],"REQ");
	else strcat(s[index1],"REP");
	strcpy(command,s[index1]);
}

int get_mode(char *s)
{
	int val,n,i;
	for(val=0,i=0; i < strlen(s); i++)
	{
		n = (int)(s[i]) - 96;
		val += n;
	}
	return val;
}
