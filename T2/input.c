#include "input.h"

#define MAX_PATH_SIZE 3000
#define MAX_SIZE 1000
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

static char root_path[MAX_PATH_SIZE];
static int level;

int get_mode(char *s);
void get_all_commands(int index, char *command);
char* get_word(int word_number, char *cmd);
char* get_exec(char *path, char *input);
int nparam(char *input);
void backward(char *path);
int count_level(char *path);
void show_man(char *cmd);
char* pretty_path(char *path);

void set_root(void)
{
	strcpy(root_path,"SFS-root-di/");
}

char* get_input(void)
{
	char *execucao;
	char input[MAX_SIZE];
	int i;
	char path[MAX_PATH_SIZE];
	set_root();
	level = count_level(root_path) - 1;
	int loop = 0;
	while(loop == 0)
	{
 		printf("Dir Atual: %s%s\n", root_path, path);
		printf("Insert a command: ");
		scanf(" %[^\n]", input);
		execucao = get_exec(path, input);
		if(execucao != NULL) loop = 1;
	}
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
	int nparam;
	exec = (char*)malloc(MAX_PATH_SIZE * sizeof(char));
	action = get_word(1,input);
	mode = get_mode(action);
	if(mode == MKDIR || mode == RMDIR)
	{
		char *dirname;
		if(mode == MKDIR) get_all_commands(3,cmd);
		else if(mode == RMDIR) get_all_commands(4,cmd);
		dirname = get_word(2, input);
		sprintf(exec, "%s,%s,%d,%s,%d",cmd,path,strlen(path),dirname, strlen(dirname));
		return exec;
	}
	else if(mode == OPENDIR)
	{
		char *newpath;
		newpath = get_word(2,input);
		if(strcmp("..", newpath)== 0) backward(path);
		else if(strcmp("root",newpath)==0) strcpy(path,root_path); 
		else sprintf(path, "%s%s/",path,newpath);
		return NULL;
	}
	else if(mode == LSALL)
	{
		get_all_commands(5,cmd);
		sprintf(exec,"%s,%s,%d",cmd,path,strlen(path));
		return exec;
	}
	else if(mode == RDFILE || mode == WRFILE || mode == MKFILE || mode == RMFILE)
	{
		char *filename;
		filename = get_word(2,input);
		if(mode == RDFILE)
		{
			get_all_commands(0,cmd);
			int nbytes = atoi(get_word(4,input));
			int offset = atoi(get_word(5,input));
			sprintf(exec,"%s,%s,%d,,%d,%d",cmd,path,strlen(path),nbytes,offset);
		}
		else
		{
			char *payload;
			char newpath[MAX_SIZE];
			get_all_commands(1,cmd);
			filename = get_word(2,input);
			sprintf(newpath, "%s%s",path, filename);
			sprintf(exec, "%s,%s,%d", cmd, newpath, strlen(newpath));
			if(mode == WRFILE)
			{
				payload = get_word(3,input);
				int nbytes = atoi(get_word(3,input));
				int offset = atoi(get_word(4,input));
				sprintf(exec,"%s,%s,%d,%d",exec,payload,nbytes,offset);
			}
			else if(mode == MKFILE)
			{
				int nbytes = atoi(get_word(3,input));
				sprintf(exec,"%s,,%d,%d",exec,nbytes,0);
			}
			else if(mode == RMFILE)
			{
				int nbytes = atoi(get_word(3,input));
				int offset = atoi(get_word(4,input));
				sprintf(exec,"%s,%d,%d",exec,0,offset);
			}
		}
		return exec;
	}
	else if(mode == SHOWINFO)
	{
		char newpath[MAX_SIZE];
		strcpy(newpath,path);
		get_all_commands(2,cmd);
		if(nparam == 3)
		{
			char *file = get_word(2,input);
			strcat(newpath,file);
		}
		sprintf(exec,"%s,%s,%d", cmd,newpath,strlen(path));
		return exec;
	}
	else if(mode == HELP)
	{
		printf("COMANDOS DO PROGRAMA: <PARAMETROS>\n");
		printf("\tmkdir\t<dirname(string)>\n");
		printf("\trmdir\t<dirname(string)>\n");
		printf("\topendir\t<dirname(string)>\n");
		printf("\tlsall\n");
		printf("\trdfile\t<file(string)> <nbytes(int)> <offset(int)>\n");
		printf("\twrfile\t<file(string)> ");
		printf("<content(string)> <nbytes(int)> <offset(int)>\n");
		printf("\tmkfile\t<file(string)> <filesize(int)>\n");
		printf("\trmfile\t<file(string)>\n");
		printf("\tshinfo\t<file(string)>\n");
		printf("\thelp\n");
		printf("\tman <command> :: MANUAL\n");
		printf("\tcmdclean\n");
		return NULL;
	}
	else if(mode == MANUAL)
	{
		show_man(get_word(2,input));
		return NULL;	
	}
	else if(mode == CMDCLEAN)
	{
		system("clear");
		return NULL;
	}
	else
	{
		printf("Input Invalido\n");
		return NULL;
	}
}

void get_all_commands(int index, char *command)
{
	char s[6][7];
	strcpy(s[0],"RD-REQ");
	strcpy(s[1],"WR-REQ");
	strcpy(s[2],"FI-REQ");
	strcpy(s[3],"DC-REQ");
	strcpy(s[4],"DR-REQ");
	strcpy(s[5],"DL-REQ");
	strcpy(command, s[index]);
}

char* get_word(int word_number, char *input)
{
	int i,j,k;
	char *action;
	char word[MAX_SIZE];
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

int get_mode(char *s)
{
	int val;
	if(strcmp(s,"mkdir")==0) val = 0;
	else if(strcmp(s,"rmdir")==0) val = 1;
	else if(strcmp(s,"opendir")==0) val = 2;
	else if(strcmp(s,"lsall")==0) val = 3;
	else if(strcmp(s,"rdfile")==0) val = 4;
	else if(strcmp(s,"wrfile")==0) val = 5;
	else if(strcmp(s,"mkfile")==0) val = 6;
	else if(strcmp(s,"rmfile")==0) val = 7;
	else if(strcmp(s,"shinfo")==0) val = 8;
	else if(strcmp(s,"help")==0) val = 9;
	else if(strcmp(s,"man")==0) val = 10;
	else if(strcmp(s,"cmdclean")==0) val = 11;
	else val = -1;
	return val;
}

int nparam(char *input)
{
	int i,n=0;
	for(i=0; i < strlen(input);i++) if(input[i] == SPACE) n++;
	return n+1;
}

void backward(char *path)
{
	int i;
	for(i=strlen(path)-2; path[i] != '/'; i--)
	{
		path[i] = '\0';
		if(i == 0) break;
	}
}

int count_level(char *path)
{
	int i,j;
	for(i=0,j=0; path[i]!='\0'; i++) if(path[i] == '/') j++;
	return j;
}

char* pretty_path(char *path)
{
	int i,j,jmp;
	char *pretty_path;
	int n = level;
	pretty_path = (char*)malloc(MAX_PATH_SIZE * sizeof(char));
	strcpy(pretty_path,path);
	for(i=0,j=0; j < n; i++) if(path[i] == '/') j++;
	jmp = i;
	for(i=jmp; path[i] != '\0'; i++)pretty_path[i-jmp] = path[i];
	for(i=(strlen(path)-jmp); i < strlen(path)+1; i++) pretty_path[i] = '\0';
	return pretty_path;
}

void show_man(char *cmd)
{
	int mode;
	mode = get_mode(cmd);
	printf("Comando: %s\n",cmd);
	if(mode == MKDIR)
	{
		printf("\tCriar um diretorio com esse nome no diretorio atual\n");
		printf("\t\tRecebe string com nome de um diretorio\n");
	}
	else if(mode == RMDIR)
	{
		printf("\tDeleta um diretorio com esse nome no diretorio atual, e tambem os sub-diretorios\n");
		printf("\t\tRecebe string com nome de um diretorio\n");
	}
	else if(mode == OPENDIR)
	{
		printf("\tAltera o diretorio atual para o diretorio recebido como parametro\n");
		printf("\t\tRecebe string com nome de um diretorio\n");
	}
	else if(mode == LSALL)
	{
		printf("\tLista tudo no diretorio atual\n");
		printf("\t\tNao recebe parametros\n");
	}
	else if(mode == RDFILE)
	{
		printf("\tLe um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
		printf("\t\t<nbytes> [int] = numero de bytes a serem lidos\n");
		printf("\t\t<offset> [int] = posicao de onde a leitura sera iniciada\n");
	}
	else if(mode == WRFILE)
	{
		printf("\tEscreve um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
		printf("\t\t<content> [string] = conteudo a ser escrito no arquivo\n");
		printf("\t\t<nbytes> [int] = numero de bytes a serem lidos\n");
		printf("\t\t<offset> [int] = posicao de onde a leitura sera iniciada\n");
	}
	else if(mode == MKFILE)
	{
		printf("\tDeleta um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
		printf("\t\t<nbytes> [int] = tamanho do arquivo a ser criado\n");
	}
	else if(mode == MKFILE)
	{
		printf("\tCria um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
	}
	else if(mode == SHOWINFO)
	{
		printf("\tExibe informacoes a respeito de um arquivo\n");
		printf("\tRecebe:\n\t\t<file> [string] = nome do arquivo\n");
	}
	else if(mode == CMDCLEAN)
	{
		printf("\tLimpa o Console\n");
	}
	else
	{
		printf("\tSinto, voce inseriu:\n");
		printf("\t\tComando InvalidoNao\n");
		printf("\t\tou\n");
		printf("\t\tExiste Manual para esse comando\n");
	}
}

