#include "input.h"
#pragma once

#define OLDCLIENT 1
#define NEWCLIENT -1

#define SPACE 32

#define MKDIR 0
#define RMDIR 1
#define OPENDIR 2
#define LSALL 3 
#define RDFILE 4
#define WRFILE 5
#define MKFILE 6
#define RMFILE 7
#define SHOWINFO 8
#define HELP 9
#define MANUAL 10
#define CMDCLEAN 11
#define EXIT 12
#define ONELIST 13
#define CHGPERM 14

static char root_path[MAX_PATH_SIZE];
static int level;
static char patho[MAX_PATH_SIZE];
static int clientID;
static int newclient = NEWCLIENT;

void insert_barra(char *s);
void remove_barra(char *s);
void start_new_client(void);
int get_mode(char *s);
void get_all_commands(int index, char *command);
char* get_word(int word_number, char *cmd);
int nparam(char *input);
char* get_exec(char *path, char *input);
void backward(char *path);
int count_level(char *path);
void show_man(char *cmd);
char* pretty_path(char *path);
void cat_clientID(char* exec, char* ID);
void tabela_permissoes(void);

void start_new_client(void)
{
	strcpy(patho, ROOT_PATH);
	clientID = NEWCLIENT;
}

void set_root(void)
{
	strcpy(root_path, "SFS-root-di");
}

char* get_input(void)
{
	char *execucao;
	char input[MAX_SIZE];
	char cID[30];
	set_root();
	level = count_level(root_path) - 1;
	int loop = 0;
	if (newclient == NEWCLIENT)
	{
		start_new_client();
		newclient = OLDCLIENT;
	}
	clientID = 145;
	if (clientID == NEWCLIENT)
	{
		printf("You need to log in before issuing any commands.\nPlease enter your ID [Numerical Values Only]: ");
		scanf("%d", &clientID);
		printf("Setting [%d] as client ID\n", clientID);
	}
	sprintf(cID, "%d", clientID);
	while (loop == 0)
	{
		printf("[%s @ %s%s]: ", cID, root_path, patho);
		scanf(" %[^\n]", input);
		execucao = get_exec(patho, input);
		if (execucao != NULL) loop = 1;
		if (clientID == NEWCLIENT) return get_input();
	}
	cat_clientID(execucao, cID);
	return execucao;
}

char *pp(char *path)
{
	return pretty_path(path);
}

char* get_exec(char *path, char *input)
{
	char *exec;
	char *action;
	char cmd[7];
	int mode;
	int npm;
	exec = (char*)malloc(MAX_PATH_SIZE * sizeof(char));
	action = get_word(1, input);
	mode = get_mode(action);
	npm = nparam(input);
	if (mode == MKDIR || mode == RMDIR)
	{
		char *dirname;
		if (mode == MKDIR) get_all_commands(3, cmd);
		else if (mode == RMDIR) get_all_commands(4, cmd);
		dirname = get_word(2, input);
		sprintf(exec, "%s,%s,%d,%s,%d", cmd, path, strlen(path), dirname, strlen(dirname)+1);
		return exec;
	}
	else if (mode == OPENDIR)
	{
		char *newpath;
		newpath = get_word(2, input);
		if (strcmp("/..", newpath) == 0) backward(path);
		else if (strcmp("/root", newpath) == 0) strcpy(path, ROOT_PATH);
		else sprintf(path, "%s%s", path, newpath);
		return NULL;
	}
	else if (mode == LSALL)
	{
		get_all_commands(5, cmd);
		sprintf(exec, "%s,%s,%d", cmd, path, strlen(path));
		return exec;
	}
	else if (mode == RDFILE || mode == WRFILE || mode == MKFILE || mode == RMFILE)
	{
		char *filename;
		filename = get_word(2, input);
		if (mode == RDFILE)
		{
			get_all_commands(0, cmd);
			char *auxiliar;
			auxiliar = get_word(3,input);remove_barra(auxiliar);
			int nbytes = atoi(auxiliar);
			auxiliar = get_word(4,input);remove_barra(auxiliar);
			int offset = atoi(auxiliar);
			sprintf(exec, "%s,%s%s,%d,,%d,%d", cmd, path,filename,strlen(path)+strlen(filename), nbytes, offset);
		}
		else
		{
			char *payload;
			char newpath[MAX_SIZE];
			get_all_commands(1, cmd);
			filename = get_word(2, input);
			sprintf(newpath, "%s%s", path, filename);
			sprintf(exec, "%s,%s,%d", cmd, newpath, strlen(newpath));
			if (mode == WRFILE)
			{
				payload = get_word(3, input);
				char *auxiliar;
				auxiliar = get_word(4,input);remove_barra(auxiliar);
				int nbytes = atoi(auxiliar);
				auxiliar = get_word(5,input);remove_barra(auxiliar);
				int offset = atoi(auxiliar);
				remove_barra(payload);
				sprintf(exec, "%s,%s,%d,%d", exec, payload, nbytes, offset);
			}
			else if (mode == MKFILE)
			{
				sprintf(exec, "%s,,%d,%d", exec, 1, 0);
			}
			else if (mode == RMFILE)
			{
				char *auxiliar;
				auxiliar = get_word(3,input);remove_barra(auxiliar);
				int nbytes = atoi(auxiliar);
				auxiliar = get_word(4,input);remove_barra(auxiliar);
				int offset = atoi(auxiliar);
				sprintf(exec, "%s,%d,%d", exec, 0, offset);
			}
		}
		return exec;
	}
	else if (mode == SHOWINFO)
	{
		char newpath[MAX_SIZE];
		strcpy(newpath, path);
		get_all_commands(2, cmd);
		if (npm == 3)
		{
			char *file = get_word(2, input);
			strcat(newpath, file);
		}
		sprintf(exec, "%s,%s,%d", cmd, newpath, strlen(path));
		return exec;
	}
	else if (mode == HELP)
	{
		printf("COMANDOS DO PROGRAMA: <PARAMETROS>\n");
		printf("\tmkdir\t<dirname(string)>\n");
		printf("\trmdir\t<dirname(string)>\n");
		printf("\topendir\t<dirname(string)>\n");
		printf("\tlsall\n");
		printf("\tls\n");
		printf("\trdfile\t<file(string)> <nbytes(int)> <offset(int)>\n");
		printf("\twrfile\t<file(string)> ");
		printf("<content(string)> <nbytes(int)> <offset(int)>\n");
		printf("\tmkfile\t<file(string)>\n");
		printf("\trmfile\t<file(string)>\n");
		printf("\tshinfo\t<file(string)>\n");
		printf("\tchperm\t<file(string)> <read_perm(int)> <write_perm(int)>");
		printf("\thelp\n");
		printf("\tman <command> :: MANUAL\n");
		printf("\texit\n");
		printf("\tcmdclean\n");
		return NULL;
	}
	else if (mode == MANUAL)
	{
		show_man(get_word(2, input));
		return NULL;
	}
	else if (mode == CMDCLEAN)
	{
		system("clear");
		return NULL;
	}
	else if (mode == EXIT)
	{
		newclient = NEWCLIENT;
		start_new_client();
		printf("\n");
		return NULL;
	}
	else if(mode == ONELIST)
	{
		get_all_commands(6, cmd);
		sprintf(exec, "%s,%s,%d", cmd, path, strlen(path));
		return exec;
	}
	else if(mode == CHGPERM)
	{
		int rp, wp;
		char *filename;
		filename = get_word(2, input);
		char *auxiliar;
		auxiliar = get_word(3,input);remove_barra(auxiliar);
		rp = atoi(auxiliar);
		auxiliar = get_word(4,input);remove_barra(auxiliar);
		wp = atoi(auxiliar);
		get_all_commands(7,cmd);
		sprintf(exec,"%s,%s,%d,%d,%d",cmd,path,strlen(path),rp,wp);
	}
	else
	{
		printf("Input Invalido\n");
		return NULL;
	}
}

void get_all_commands(int index, char *command)
{
	char s[10][7];
	strcpy(s[0], "RD-REQ");
	strcpy(s[1], "WR-REQ");
	strcpy(s[2], "FI-REQ");
	strcpy(s[3], "DC-REQ");
	strcpy(s[4], "DR-REQ");
	strcpy(s[5], "DL-REQ");
	strcpy(s[6], "OL-REQ");
	strcpy(s[7], "CL-REQ");
	strcpy(command, s[index]);
}

char* get_word(int word_number, char *input)
{
	int i, j, k,m;
	char *action;
	char word[MAX_SIZE];
	m = strlen(input) + 1;
	for (i = 0, j = 0, k = 0; i < m; i++, k++)
	{
		word[k] = input[i];
		if (input[i] == SPACE || input[i] == '\0')
		{
			word[k] = '\0';
			j++;
			k = -1;
			if (word_number == j) break;
		}
	}
	action = (char*)malloc(strlen(word)+1 * sizeof(char));
	strcpy(action, word);
	if(word_number > 1) insert_barra(action);
	return action;
}

int get_mode(char *s)
{
	int val;
	if (strcmp(s, "mkdir") == 0) val = 0;
	else if (strcmp(s, "rmdir") == 0) val = 1;
	else if (strcmp(s, "opendir") == 0) val = 2;
	else if (strcmp(s, "lsall") == 0) val = 3;
	else if (strcmp(s, "rdfile") == 0) val = 4;
	else if (strcmp(s, "wrfile") == 0) val = 5;
	else if (strcmp(s, "mkfile") == 0) val = 6;
	else if (strcmp(s, "rmfile") == 0) val = 7;
	else if (strcmp(s, "shinfo") == 0) val = 8;
	else if (strcmp(s, "help") == 0) val = 9;
	else if (strcmp(s, "man") == 0) val = 10;
	else if (strcmp(s, "cmdclean") == 0) val = 11;
	else if (strcmp(s, "exit") == 0) val = 12;
	else if (strcmp(s, "ls") == 0) val = 13;
	else if (strcmp(s, "chperm") == 0) val = 14;
	else val = -1;
	return val;
}

int nparam(char *input)
{
	int i, n = 0;
	int m = strlen(input);
	for (i = 0; i < m; i++) if (input[i] == SPACE) n++;
	return n + 1;
}

void backward(char *path)
{
	int i;
	printf("\n");
	for (i = strlen(path); path[i] != '/'; i--)
	{
		path[i] = '\0';
		if (i == 0) break;
	}
	path[i] = '\0';
}

int count_level(char *path)
{
	int i, j;
	for (i = 0, j = 0; path[i] != '\0'; i++) if (path[i] == '/') j++;
	return j-1;
}

char* pretty_path(char *path)
{
	int i, j, jmp;
	char *pretty_path;
	int n = level;
	int m = strlen(path) + 1;
	pretty_path = (char*)malloc(MAX_PATH_SIZE * sizeof(char));
	strcpy(pretty_path, path);
	for (i = 0, j = 0; j < n; i++) if (path[i] == '/') j++;
	jmp = i;
	for (i = jmp; path[i] != '\0'; i++)pretty_path[i - jmp] = path[i];
	for (i = (strlen(path) - jmp); i < m; i++) pretty_path[i] = '\0';
	return pretty_path;
}

void show_man(char *cmd)
{
	int mode;
	remove_barra(cmd);
	mode = get_mode(cmd);
	printf("Comando: %s\n", cmd);
	if (mode == MKDIR)
	{
		printf("\tCria um diretorio com esse nome no diretorio atual\n");
		printf("\t\tRecebe string com nome do diretorio criado\n");
	}
	else if (mode == RMDIR)
	{
		printf("\tDeleta um diretorio com esse nome no diretorio atual, e tambem os sub-diretorios\n");
		printf("\t\tRecebe string com nome de um diretorio\n");
	}
	else if (mode == OPENDIR)
	{
		printf("\tAltera o diretorio atual para o diretorio recebido como parametro\n");
		printf("\t\tRecebe string com nome de um diretorio\n");
		printf("\t\t [ .. ] retornara para o diretorio pai\n");
		printf("\t\t [ root ] retornara para o diretorio root\n");
	}
	else if (mode == LSALL)
	{
		printf("\tLista tudo no diretorio atual e seus sub-diretorios\n");
		printf("\t\tNao recebe parametros\n");
	}
	else if (mode == RDFILE)
	{
		printf("\tLe um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
		printf("\t\t<nbytes> [int] = numero de bytes a serem lidos\n");
		printf("\t\t<offset> [int] = posicao de onde a leitura sera iniciada\n");
	}
	else if (mode == WRFILE)
	{
		printf("\tEscreve um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
		printf("\t\t<content> [string] = conteudo a ser escrito no arquivo\n");
		printf("\t\t<nbytes> [int] = numero de bytes a serem lidos\n");
		printf("\t\t<offset> [int] = posicao de onde a leitura sera iniciada\n");
	}
	else if (mode == MKFILE)
	{
		printf("\tDeleta um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
		printf("\t\t<nbytes> [int] = tamanho do arquivo a ser criado\n");
	}
	else if (mode == MKFILE)
	{
		printf("\tCria um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
	}
	else if (mode == SHOWINFO)
	{
		printf("\tExibe informacoes a respeito de um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
	}
	else if (mode == CMDCLEAN)
	{
		printf("\tLimpa o Console\n");
	}
	else if (mode == EXIT)
	{
		printf("\tLog Off do Usuario\n");
	}
	else if(mode == ONELIST)
	{
		printf("\tLista tudo apenas no diretorio atual\n");
		printf("\t\tNao recebe parametros\n");
	}
	else if(mode == CHGPERM)
	{
		printf("\tAltera as permissoes de leitura de um diretorio\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
		printf("\t\t<read_perm> [int] = permissao para leitura\n");
		printf("\t\t<write_perm> [int] = permissao para leitura\n");
		tabela_permissoes();
	}
	else
	{
		printf("\tSinto, voce inseriu:\n");
		printf("\t\tComando Invalido\n");
		printf("\t\tou\n");
		printf("\t\tNao existe Manual para esse comando\n");
		printf("Mode = %d\n",mode);
	}
}

void cat_clientID(char* exec, char* ID)
{
	char temp[MAX_PATH_SIZE];
	sprintf(temp, "%s,%s", ID, exec);
	strcpy(exec, temp);
}

char* teste_input(FILE *arq,int n)
{
	char *execucao;
	char input[MAX_SIZE];
	char cID[30];
	set_root();
	level = count_level(root_path) - 1;
	clientID = 9999;
	sprintf(cID, "%d", clientID);
	printf("[%s @ %s%s]: ", cID, root_path, patho);
	fscanf(arq," %[^\n]", input);
	printf("%s\n", input);
	execucao = get_exec(patho, input);
	if (clientID == NEWCLIENT) return get_input();
	cat_clientID(execucao, cID);
	return execucao;
}

void insert_barra(char *s)
{
	char *aux;
	aux = (char*)malloc(sizeof(char)* MAX_SIZE);
	sprintf(aux,"/%s",s);
	strcpy(s,aux);
}
void remove_barra(char *s)
{
	int i,f;
	f = strlen(s)+1;
	for(i=0; i < f; i++)
	{
		s[i] = s[i+1];
	}
}

void tabela_permissoes(void)
{
	int i,j;
	printf("\t\tPermissoes:\n");
	printf("\t\tQuem pode ler:\n");
	printf("\t\t\tOwner e Usuario:\t0\n");
	printf("\t\t\tOwner Apenas:\t2\n");
	printf("\t\t\tUsuario Apenas:\t1\n");
	printf("\t\t\tNinguem:\t3\n");
}
