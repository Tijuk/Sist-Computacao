#pragma once
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <sys/param.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


#define MP 300
#define MPATH 100
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
	int filelength;
	Ar lst[40];
}Info;

typedef enum dir_returns
{
    D_OK = 0,
    D_MemoInsufic = 1,
    D_AcessoNegado = 2,
    D_DirecVazio = 3,
    D_DirecNaoEncontrado = 4,
    D_NotDiretorio = 5,
}D_Ret;

/*FUNCOES DE LEITURA*/
char* scpy(char *s2);
void exibe_code(Info *a);
int check_code(char *code);
void get_args(char *s, Args *ar);
int read_code(char *s, Info *a, Args *ar);
void reader(int mode, Args *ar, Info *a);
void viewer(int mode, Info *a);
void teste(Info *a, Args *ar);
void get_decision(Info *a, char*cmdClient);
/*---------------------*/

/*FUNCOES DE MANIPULACAO DE DIRETORIOS*/
char* removeslash(char *s);
int remove_directory(const char *path);
int file_select(struct direct *entry);
D_Ret liststuff(char *path);
D_Ret checkpathright(char *pathname, char *findpath);
D_Ret createdirectory(char *path, int path_len, char* dirname, int dir_len);
D_Ret deletedirectory(char *path, int path_len, char* dirname, int dir_len);
D_Ret acessdirectory(char *pathname, char *path);
void go_home(char *path, char *root);
/*---------------------*/

void get_command(Info *a, char * lpath);
