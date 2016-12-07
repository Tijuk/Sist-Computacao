#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/xattr.h>

extern int alphasort();
int clienteID;

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

void get_command(Info *a, char* lpath);

void get_decision(Info *a,char *cmdClient);
char root_path[MP];


char* removeslash(char *s)
{
	int i,n;
	char* ret;
	n = strlen(s);
	ret = (char*)malloc(n*sizeof(char));
	for(i=0; i<n;i++)
	{
		ret[i] = s[i+1];
	}
	return ret;
}
/*Criar Diretorio*/
D_Ret createdirectory(char *path, int path_len, char* dirname, int dir_len)
{
	int status;
	char *dirn;
	char *path2;
	char *oldpath;
	FILE *fp;
	
	struct stat buffer;
	
	oldpath = (char*)malloc(200*sizeof(char));
	//Salvar o path origem
	getcwd(oldpath,200*sizeof(char));

	printf("Path do diretorio que queremos adicionar %s\n",path);

	//Entrar no diretorio descrito no path
	chdir(path);

	//Gravar o path atual do diretorio
	path2 = (char*)malloc(200*sizeof(char));
	getcwd(path2,200*sizeof(char));
	
	printf("Creating: %s at %s\n",dirname,path2);
	//Criar o diretorio	
	status = mkdir(dirname,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//Gravar no diretorio criado o id do cliente
	char d1,codtemp1 = 0x1; //client
	char d2,codtemp2 = 0x2; //permissao

	int ret1,ret2,ret3,ret4;
	ret1 = setxattr(dirname,"user.client",&codtemp1,1,0);
	ret3 = setxattr(dirname,"user.permissao",&codtemp2,1,0);

	if(ret1 < 0)
		printf("erro1\n");
	if(ret3 < 0)
		printf("erro2\n");

	ret2=getxattr(dirname,"user.client",&d1,1);
	ret4=getxattr(dirname,"user.permissao",&d2,1);

	if(ret4 < 0)
		printf("erro3\n"); 	
	if(ret2 < 0)
		printf("erro4\n");

	printf("Client id:%d\n",d1);
	printf("Permissao:%d\n",d2);
	//Voltar pro diretorio de origem
	chdir(oldpath);


	if(status==0)
	{
		return D_OK;
	}
	else
	{
		return D_AcessoNegado;
	}	

}
/*Deletar Diretorio*/
D_Ret deletedirectory(char *path, int path_len, char* dirname, int dir_len)
{
	int i;
	char *dirn;
	int status;
	//dirn = removeslash(dirname);
	dirn = dirname;	
	chdir(path);
	remove_directory(dirn);
	printf("%s was deleted!!\n", dirn);
	if(status==0)
	{
		return D_OK;
	}
	else
	{
		return D_AcessoNegado;
	}
}

int file_select(struct direct *entry)
{
	if((strcmp(entry->d_name,".")== 0 || strcmp(entry->d_name,"..") == 0))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void go_home(char *path, char *root)
{
	char pathname[MP];
	while(1)
	{
		getcwd(pathname,MP);
		if(strcmp(pathname,root)==0)
		{
			return;
		}
		chdir("..");
	}
}

void get_command(Info *a, char * lpath){

    	if(strcmp(a->code,"DC-REQ")==0)
	{
		createdirectory(lpath,0, a->path2, a->path2len);
		chdir(root_path);
	}
	else if(strcmp(a->code,"DR-REQ")==0)
	{
		deletedirectory(lpath,0, a->path2, a->path2len);
		chdir(root_path);
	}
	else if(strcmp(a->code,"DL-REQ")==0)
	{
		printf("%s\n",lpath);
		liststuff(lpath);
	}
}
/*Lista Diretorio*/
D_Ret liststuff(char *pathname)
{
    int count,i;
    static struct direct **files;
    static struct stat status;
    int file_select();

    printf("pathname que vou ler:%s\n",pathname);
    
    count = scandir(pathname, &files, file_select, alphasort);
    if (count <= 0)
    {
        printf("No files in this directory\n");
        return D_DirecVazio;
    }
    printf("Number of files = %d\n",count);
    for (i=1;i<count+1;++i)
    {
        stat(files[i-1]->d_name, &status);
	if(files[i-1]->d_type == DT_DIR)
		printf("Diretorio ===> %s\n",files[i-1]->d_name);
	else
        	printf("Arquivo ==========> %s\n", files[i-1]->d_name);
    }
    printf("\n");
    return D_OK;
}

D_Ret checkpathright(char *pathname, char *findpath)
{
    int count,i;
    static struct direct **files;
    static struct stat status;
    int file_select();
    count = scandir(pathname, &files, file_select, alphasort);
    if (count <= 0)
    {
        printf("No files in this directory\n");
        return D_DirecVazio;
    }
    printf("Number of files = %d\n",count);
    for (i=1;i<count+1;++i)
    {
        stat(files[i-1]->d_name, &status);
        if(strcmp(files[i-1]->d_name,findpath) == 0)
        {
            return D_OK;
        }
    }
    printf("Path not found!\nShame on you!\n");
    return D_DirecNaoEncontrado;
}

void get_decision(Info *a,char *cmdClient)
{
	char s[MP];
	Args *ar;
	int code;
	ar = (Args*)malloc(sizeof(Args));

	get_args(cmdClient,ar);
	read_code(cmdClient,a,ar);
}

/*Remover diretorio*/
int remove_directory(const char *path)
{
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;
	if (d)
	{
		struct dirent *p;
		r = 0;
		while (!r && (p=readdir(d)))
		{
			int r2 = -1;
			char *buf;
			size_t len;
			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
			{
				continue;
			}
			len = path_len + strlen(p->d_name) + 2;
			buf = malloc(len);
			if (buf)
			{
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf))
				{
					if (S_ISDIR(statbuf.st_mode))
					{
						r2 = remove_directory(buf);
					}
					else
					{
						r2 = unlink(buf);
					}
				}
				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}
	if (!r)
	{
		r = rmdir(path);
	}
	return r;
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
	int code;
	a->code = scpy(ar->arg[0]);
	code = check_code(ar->arg[0]);
	if(code < 12)
	{
		reader(0,ar,a);
	}
	if(code == 0 || code < 4)
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
		return 0;
	}
}
void reader(int mode, Args *ar, Info *a)
{
	switch(mode)
	{
		case 0:
		{
			a->path = scpy(ar->arg[1]);
			a->pathlen = strlen(ar->arg[1]);
			break;
		}
		case 1:
		{
			a->payload = scpy(ar->arg[3]);
			a->nrbytes = atoi(ar->arg[4]);
			a->offset = atoi(ar->arg[5]);
			break;
		}
		case 2:
		{
			a->path2 = scpy(ar->arg[3]);
			a->path2len = strlen(ar->arg[3]);
		}
		case 3:
		{
			a->owner = atoi(ar->arg[3]);
			a->permissions = scpy(ar->arg[4]);
			a->filelength = atoi(ar->arg[5]);
		}
	}
}

void exibe_code(Info *a)
{
	int i;
	int code = check_code(a->code);
	if(code < 12)
	{
		viewer(0,a);
	}
	if(code == 0 || code < 4)
	{
		viewer(1,a);
	}
	else if(code == 4)
	{
		viewer(3,a);
	}
	else if(code == 5 || code == 6)
	{
		viewer(2,a);
	}
	else if(code == 7)
	{
		return;
	}
}

void viewer(int mode, Info *a)
{
	switch(mode)
	{
		case 0:
		{
			printf("Code: %s\n",a->code);
			printf("Path: %s\n",a->path);
			printf("PathLeng: %d\n",a->pathlen);
			break;
		}
		case 1:
		{
			printf("PayLoad: %s\n",a->payload);
			printf("NrBytes: %d\n",a->nrbytes);
			printf("Offset: %d\n",a->offset);
			break;
		}
		case 2:
		{
			printf("DirName: %s\n",a->path2);
			printf("DirNameLeng: %d\n",a->path2len);
			break;
		}
		case 3:
		{
			printf("Owner: %d\n",a->owner);
			printf("Permissions: %s\n",a->permissions);
			printf("File Length: %d\n",a->filelength);
			break;
		}
	}
}

int check_code(char *code)
{
	int i;
	int cmp;
	char codelist[12][7];
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
		cmp = strcmp(code,codelist[i]);
		if(cmp == 0)
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
