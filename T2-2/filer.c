#include "filer.h"

typedef struct file_manager_strt{
	int clientID;
	char *path;
	char *filename;
	char *payload;
	int nrbytes;
	int offset;
	int write_perm,read_perm;
	int owner;
}Filer;

#define TRUE 0
#define FALSE 1

/*
write_perm	user	owner
	0	 W	  W
	1	 W	 NW
	2	NW	  W
	3	NW	 NW

read_perm	user	owner
	0	 R	  R
	1	 R	 NR
	2	NR	  R
	3	NR	 NR
*/

Filer* fill_file(int ID, char *path,char *payload, int *nrbytes, int offset);
char* scpy(char *s2);
void checa_erro(int erro, char *s);
void return_values(Filer* f, char *payload, int* nrbytes);
void cut_string(char *s, int wanted_size);

int write_file(Filer *file);
int read_file(Filer *file);
int create_file(Filer *file);
int delete_file(Filer *file);
int check_permission(int clientID, int owner, int perm);

int file_manipulation(int client, int code,char *path,char *payload, int *nrbytes, int offset)
{
	Filer *file;
	int value = -1;
	char present_path[MP];
	char aux[MP];
	getcwd(present_path,MP*sizeof(char));
	sprintf(aux,"%s/%s%s",present_path,FULL_ROOT,path);
	file = fill_file(client,aux,payload,nrbytes,offset);
	if(code == 0) // read
	{
		value = read_file(file);
		payload = scpy(file->payload);
	}
	else if(code == 1) // write or create or delete
	{
		if(file->nrbytes == 0)
		{
			printf("Deletando arquivo: %s\n",file->path);
			value = delete_file(file);
		}
		else if(strlen(file->payload)==0)
		{
			printf("Criando arquivo: %s\n",file->path);
			value = create_file(file);
		}
		else
		{
			printf("Escrevendo no arquivo: %s\n", file->path);
			value = write_file(file);		
		}
	}
	if(value == 1) printf("Not enough permissions bro\n");
	*nrbytes = file->nrbytes;
	return value;
}

int create_file(Filer *file)
{
	int fd;
	int permission;
	char s = 32;
	permission = check_permission(file->clientID, file->owner, file->write_perm);
	if(permission == TRUE)
	{
		fd = open(file->path, O_RDWR|O_CREAT,0666);
		if(fd < 0)
		{
			printf("Erro abertura do arquivo\n");
			return 1;
		}
		write(fd,&s, sizeof(char));
		return 0;
	}
	file->offset = -1;
	return 1;
}

int delete_file(Filer *file)
{
	int permission;
	permission = check_permission(file->clientID, file->owner, file->write_perm);
	if(permission == TRUE)
	{
		remove(file->path);
		return 0;
	}
	file->offset = -1;
	return 1;
}

int write_file(Filer *file)
{
	int permission = check_permission(file->clientID, file->owner, file->write_perm);
	if(permission == TRUE)
	{
		char* cutted_payload = scpy(file->payload);
		FILE* arq;
		cut_string(cutted_payload, file->nrbytes);
		arq = fopen(file->path,"r+");
		if(arq==NULL)
		{
			printf("Fail to open file <%s> for writing\n", file->path);
		}
		fseek(arq,file->offset,SEEK_SET);
		fprintf(arq,"%s",cutted_payload);
		rewind(arq);
		fclose(arq);
		return 0;
	}
	file->offset = -1;
	return 1;
}

int read_file(Filer *file)
{
	struct stat sb;
	off_t len;
	char *p;
	int fd;
	int form = 0;
	int countbyter = 0;
	int permission = check_permission(file->clientID, file->owner, file->read_perm);
	if(permission == TRUE)
	{
		file->payload = (char*)malloc(sizeof(char)* (file->nrbytes + 1));
		fd = open(file->path, O_RDONLY);
		checa_erro(fd,"open");
		checa_erro(fstat(fd,&sb),"fstat");
		if(!S_ISREG (sb.st_mode))
		{
			fprintf(stderr, "%s is not a file\n", file->path);
			exit(1);
		}
		p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
		if(p == MAP_FAILED)
		{
			perror("mmap");
			exit(1);
		}
		checa_erro(close(fd),"close");
		for(len = 0; len < sb.st_size; len++)
		{
			if(countbyter == file->nrbytes)
			{
				form = 1;
				break;
			}
			file->payload[len] = p[len + file->offset];
			countbyter++;
		}
		checa_erro(munmap(p,sb.st_size),"munmap");
		file->nrbytes = countbyter;
		return 0;
	}
	file->offset = -1;
	return 1;
}

void checa_erro(int erro, char *s)
{
	if(erro == -1)
	{
		perror(s);
		printf("Erro no %s\n",s);
		exit(1);
	}
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

Filer* fill_file(int ID, char *path, char *payload, int *nrbytes, int offset)
{
	int aux;
	Filer *f;
	aux = *nrbytes;
	f = (Filer*)malloc(sizeof(Filer));
	f->clientID = ID;
	f->path = scpy(path);
	if(payload != NULL) f->payload = scpy(payload);
	f->nrbytes = aux;
	f->offset = offset;
	f->read_perm = 0; //user may read
	f->write_perm = 0; //usar may write
	f->owner = 0;
	return f;
}

int check_permission(int clientID, int owner, int perm)
{
	if(clientID == owner)
	{
		return perm%2;
	}
	else
	{
		return perm/2;
	}
	return -1;
}
	
void cut_string(char *s, int wanted_size)
{
	int actual_size = strlen(s);
	if(actual_size > wanted_size)
	{
		s[wanted_size] = '\0';
	}
	return;
}
	

	
	
	
