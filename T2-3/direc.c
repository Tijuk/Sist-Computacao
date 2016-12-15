#include "direc.h"
#include "encode.c"

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
	int read_perm;
	int write_perm;
	int filelength;
}Info;

extern int alphasort();
static int clienteID;
static char aux_root_path[MP];
static int RET_STAT = 0;

// ACTIONS //
	// DIRECTORIES //
int createdirectory(char *path, int path_len, char* dirname, int dir_len);
int deletedirectory(char *path, int path_len, char* dirname, int dir_len);
	// ----------- //

	// ALL //
int change_permissions(Info *a, char *path, int permission1,int permission2);
int liststuff(char *pathname);
	// --- //
// ------- //
void cat_stat(char* exec);
char* get_file_name(char *path);
int file_select(struct direct *entry);
void go_home(void);
void get_decision(Info *a,char *cmdClient);
int remove_directory(const char *path);
void get_args(char *s, Args *ar);
void reader(int mode, Args *ar, Info *a);
int read_code(char *s, Info *a, Args *ar);
int check_code(char *code);
void exibe_args(Args *ar);
int cmp_size(char *s1, char *s2, int max);
char* get_code_REP(int code);
char* get_answer(Info *a, char* lpath);
int list_em_all(char *pathname);

char* execute_cmd(char *input)
{
	int ret;
	Info *a;
	char largepath[MP];
	char *answer;
	a = (Info*)malloc(sizeof(Info));
	get_decision(a,input);
	sprintf(largepath,"%s%s",aux_root_path,a->path);
	answer = get_answer(a,largepath);
	cat_stat(answer);
	return answer;
}

char* get_answer(Info *a, char* lpath)
{
	int n;
	int ret;
	int code = check_code(a->code);
	char *s = get_code_REP(code);
	char *answer = (char*)malloc(sizeof(char)*CMAX);
	char help[MP];
	char pload[CMAX];
	sprintf(help,"%s,%s,%d",s,a->path,a->pathlen);
	n = a->nrbytes;
	if(code == DEF_RD) //read file
	{
		ret=file_manipulation(clienteID, code, a->path,pload, &n, a->offset);
		a->nrbytes = n;
		a->payload = scpy(pload);
		printf("Direc: payload: %s\n", a->payload);
		sprintf(answer,"%s,%s,%d,%d",help,a->payload, a->nrbytes, a->offset);
		if(ret == 1)
		{
			printf("Error reading file\n");	
		}
		return answer;
	}
	else if(code == DEF_WR) //write on file
	{
		ret=file_manipulation(clienteID, code, a->path,a->payload, &n, a->offset);
		a->nrbytes = n;
		sprintf(answer,"%s,%s,%d,%d",help,a->payload,a->nrbytes,a->offset);
		if(ret == 1)
		{
			printf("Error writing on file\n");
		}
		return answer;	
	}
	else if(code == DEF_FI) //find information on file
	{
		sprintf(answer,"%s,%d,%d%d,%d",help,a->owner,a->read_perm,a->write_perm,a->filelength);
		if(ret == 1)
		{
			printf("Error finding information on file\n");
		}
		return answer;
	}
	else if(code == DEF_DC) //create directory
	{
		ret = createdirectory(lpath,a->pathlen, a->path2, a->path2len);
		chdir(aux_root_path);
		strcat(a->path,a->path2);
		sprintf(answer,"%s,%s,%d",s, a->path, strlen(a->path));
		if(ret == 1) //creation failure
		{
			printf("Error creating directory\n");
		}
		return answer;
	}
	else if(code == DEF_DR) //remove directory
	{
		ret = deletedirectory(lpath,a->pathlen, a->path2, a->path2len);
		chdir(aux_root_path);
		strcat(a->path,a->path2);
		sprintf(answer,"%s,%s,%d",s, a->path, strlen(a->path));
		if(ret == 1) //remotion failure
		{
			printf("Error deleting directory\n");
		}
		return answer;
	}
	else if(code == DEF_DL) //list directory
	{
		start_code(lpath);
		list_em_all(lpath);
		answer = encode();
		if(ret == 1) //listing failure
		{
			printf("Error listing director(y/ies)\n");
		}
		return answer;
	}
	else if(code == DEF_OL) //One-Dir list
	{
		start_code(lpath);
		list_em_all(lpath);
		answer = solo_encode();
		if(ret == 1) //listing failure
		{
			printf("Error listing one directory\n");
		}
		return answer;	
	}
	printf("Invalid Code: %s\n", s);
	return NULL;
}

int change_permissions(Info *a, char *path, int perm1,int perm2)
{
	int ret1,ret2,ret3;

	ret1=setxattr(path,"user.client",&clienteID,1,0);
	ret2=setxattr(path,"user.read_perm",&perm1,1,0);
	ret3=setxattr(path,"user.write_perm",&perm2,1,0);
/*
	if(ret1 < 0)
		printf("erro1\n");
	if(ret1 < 0)
		printf("erro2\n");
	if(ret1 < 0)
		printf("erro3\n");*/

	return 1;
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

int list_em_all(char *pathname)
{
	DIR *dir;
	int i;
	char newpath[MP];
	struct dirent *ent;
	struct direct **files; 
	struct stat status;
	int filer = 0;
	int file_select();
	if (!(dir = opendir(pathname)))
	{
		return 0;
	}
	while(1)
	{
		if (!(ent = readdir(dir)))
		{
			return 0;
		}
		stat(ent->d_name, &status);
		if(ent->d_type == DT_DIR)
		{
			if(file_select(ent) == 1)
			{
				sprintf(newpath,"%s/%s",pathname,ent->d_name);
				ins_path(newpath);
				list_em_all(newpath);
			}
		}
		else
		{
			sprintf(newpath,"%s/%s",pathname,ent->d_name);
			ins_path(newpath);
		}
	}
}

int solo_list(char *pathname)
{
	DIR *dir;
	int i;
	char newpath[MP];
	struct dirent *ent;
	struct direct **files; 
	struct stat status;
	int filer = 0;
	int file_select();
	if (!(dir = opendir(pathname)))
	{
		return 0;
	}
	while(1)
	{
		if (!(ent = readdir(dir)))
		{
			return 0;
		}
		stat(ent->d_name, &status);
		if(file_select(ent) == 1)
		{
			sprintf(newpath,"%s/%s",pathname,ent->d_name);
			ins_path(newpath);
		}
	}
}

int createdirectory(char *path, int path_len, char* dirname, int dir_len)
{
	int status;
	char *dirn;
	FILE *fp;
	struct stat buffer;
	chdir(path);
	dirname = get_file_name(dirname);
	//printf("Creating: %s at %s\n",dirname,path);
	status = mkdir(dirname,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	chdir(aux_root_path);
	if(status==0)
	{
		return 0;
	}
	else
	{
		return 1;
	}	

}

int deletedirectory(char *path, int path_len, char* dirname, int dir_len)
{
	int i;
	char *dirn;
	int status;
	dirn = get_file_name(dirname);	
	chdir(path);
	remove_directory(dirn);
	printf("%s was deleted!!\n", dirn);
	if(status==0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
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

int cmp_size(char *s1, char *s2, int max)
{
	int sum = strlen(s1) + strlen(s2) + 1;
	if (sum > max)
	{
		printf("String: %s e %s nao cabem dentro do vetor esperado\n",s1,s2);
		return 1;
	}
	return 0;
}

void exibe_args(Args *ar)
{
	int i;
	for(i=0; i < ar->nargs; i++)
	{
		printf("Arg: %d = %s\n", i, ar->arg[i]);
	}
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


void go_home(void)
{
	char pathname[MP];
	while(1)
	{
		getcwd(pathname,MP);
		if(strcmp(pathname,aux_root_path)==0)
		{
			return;
		}
		chdir("..");
	}
}

void define_root(void)
{
	char aux[MP];
	getcwd(aux, MP*sizeof(char));
	sprintf(aux_root_path,"%s/%s",aux,FULL_ROOT);
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

char* get_file_name(char *path)
{
	char* file;
	int i, j;
	file = (char*)malloc(MP * sizeof(char));
	i = j = 0;
	for (i = strlen(path); path[i] != '/'; i--);
	i++;
	for (; path[i] != '\0'; i++, j++)
	{
		file[j] = path[i];
	}
	file[j] = '\0';
	return file;
}

int check_code(char *code)
{
	int i;
	int cmp;
	char codelist[7][7];
	strcpy(codelist[0],"RD-REQ");
	strcpy(codelist[1],"WR-REQ");
	strcpy(codelist[2],"FI-REQ");
	strcpy(codelist[3],"DC-REQ");
	strcpy(codelist[4],"DR-REQ");
	strcpy(codelist[5],"DL-REQ");
	strcpy(codelist[6],"OL-REQ");
	for(i=0; i < 7; i++)
	{
		cmp = strcmp(code,codelist[i]);
		if(cmp == 0)
		{
			return i;
		}
	}
	return -1;
}

void reader(int mode, Args *ar, Info *a)
{
	switch(mode)
	{
		case -1:
		{
			a->path = scpy(ar->arg[2]);
			a->pathlen = atoi(ar->arg[3]);
			break;
		}
		case DEF_RD:
		{
			a->nrbytes = atoi(ar->arg[5]);
			a->offset = atoi(ar->arg[6]);
			break;
		}
		case DEF_WR:
		{
			a->payload = scpy(ar->arg[4]);
			a->nrbytes = atoi(ar->arg[5]);
			a->offset = atoi(ar->arg[6]);
			break;
		}
		case DEF_DC:
		{
			a->path2 = scpy(ar->arg[4]);
			a->path2len = atoi(ar->arg[5]);
			break;
		}
		case DEF_DR:
		{
			a->path2 = scpy(ar->arg[4]);
			a->path2len = atoi(ar->arg[5]);
			break;
		}
	}
}
int read_code(char *s, Info *a, Args *ar)
{
	int i;
	int code;
	clienteID = atoi(ar->arg[0]);
	a->code = scpy(ar->arg[1]);
	code = check_code(ar->arg[1]);
	reader(-1,ar,a);
	if(code!= DEF_FI || code!= DEF_DL) reader(code,ar,a);
	return 0;
}

char* get_code_REP(int code)
{
	char *s;
	char codes[7][8];
	s = (char*)malloc(sizeof(char)*8);
	strcpy(codes[DEF_RD],"RD_REP");
	strcpy(codes[DEF_WR],"WR_REP");
	strcpy(codes[DEF_FI],"FI_REP");
	strcpy(codes[DEF_DC],"DC_REP");
	strcpy(codes[DEF_DR],"DR_REP");
	strcpy(codes[DEF_DL],"DL_REP");
	strcpy(codes[DEF_OL],"OL_REP");
	strcpy(s,codes[code]);
	return s;
}
void cat_stat(char* exec)
{
	char temp[MAX_PATH_SIZE];
	sprintf(temp, "%d,%s", RET_STAT, exec);
	strcpy(exec, temp);
}
