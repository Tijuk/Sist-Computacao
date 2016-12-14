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

static int level1;


static char root_path2[MAX_PATH_SIZE];
Filer* fill_file(int ID, char *path,char *payload, int *nrbytes, int offset);
char* scpy(char *s2);
void checa_erro(int erro, char *s);
void return_values(Filer* f, char *payload, int* nrbytes);
void cut_string(char *s, int wanted_size);
void remove_barra1(char *s);

int write_file(Filer *file);
int read_file(Filer *file);
int create_file(Filer *file);
int delete_file(Filer *file);
int check_permission(char *path,int clientID);
char* pretty_path2(char *path);
int count_level2(char *path);


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
	permission = check_permission(file->path,file->clientID);

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
	permission = check_permission(file->path,file->clientID);

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
	int permission = check_permission(file->path,file->clientID);
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
	int permission = check_permission(file->clientID, file->owner, file->read_perm);
	if(permission == TRUE)
	{
		FILE *read_arq;
		int i=0;
		struct stat sb;
		char carac = 0;
		char aux[CMAX];
		read_arq = fopen(file->path, "r");
		if(read_arq == NULL)
		{
			printf("Erro na abertura do arquivo < %s > para leitura\n",file->path);
			return 1;
		}
		fseek(read_arq, file->offset, SEEK_SET);
		while(carac != EOF && (i+1) != file->nrbytes)
		{
			carac = fgetc(read_arq);
			aux[i] = carac;
			i++;
		}
		printf("--ok\n");
		aux[i] = '\0';
		file->nrbytes = i-1;
		file->payload = scpy(aux);
		return 0;
	}
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

int check_permission(char *path,int clientID)
{	
	
	int WP = 0;
	int RP = 0;
	int idClient = 0;
	int ret1,ret2,ret3;

	int c1 = 11;
	int c2 = 12;
	int c3 = 21;
	int c4 = 22;

	getcwd(root_path2,MP*sizeof(char));
	printf("Root path %s",root_path2);
	level1 = count_level2(root_path2);
	char * newpath = pretty_path2(path);

	printf("path recebido %d\n %s\n",level1,newpath);

	ret1=getxattr("SFS-root-di","user.client",&idClient,1);
	ret2=getxattr(newpath,"user.read_perm",&WP,1);
	ret3=getxattr(newpath,"user.write_perm",&RP,1);


	if(ret1 < 0){
		printf("erro1\n"); 
		//printf("path recebido %s",newpath);
	}	
	if(ret2 < 0){
		printf("erro2\n");
		//printf("path recebido %s",newpath);
	}
	if(ret2 < 0){
		printf("erro3\n");
		//printf("path recebido %s",newpath);
	}

	if(idClient == clientID) //O cliente eh o dono do file
	{
		if(WP == 0 || WP == 2)
			if(RP == 0 || RP == 2)
				return c1; // W  R
			else
				return c2;// W nR
		else
			if(RP == 0 || RP == 2)
				return c3; // nW R
			else
				return c4;// nW nR
	}
	else // O cliente eh o usuario
	{		
		if(WP == 1 || WP == 2)
			if(RP == 1 || RP == 2)
				return c1;//W R
			else
				return c2;// W nR
		else
			if(RP == 1 || RP == 2)
				return c3;//nW R
			else
				return c4;//nW nR
	}
	return TRUE;
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
void remove_barra1(char *s)
{
	int i,f;
	f = strlen(s)+1;
	for(i=0; i < f; i++)
	{
		s[i] = s[i+1];
	}
}
char* pretty_path2(char *path)
{
	int i, j, jmp;
	char *pretty_path;
	int n = level1;
	int m = strlen(path) + 1;
	pretty_path = (char*)malloc(MAX_PATH_SIZE * sizeof(char));
	strcpy(pretty_path, path);
	for (i = 0, j = 0; j < n; i++) if (path[i] == '/') j++;
	jmp = i;
	for (i = jmp; path[i] != '\0'; i++)pretty_path[i - jmp] = path[i];
	for (i = (strlen(path) - jmp); i < m; i++) pretty_path[i] = '\0';
	return pretty_path;
}
int count_level2(char *path)
{
	int i, j;
	for (i = 0, j = 0; path[i] != '\0'; i++) if (path[i] == '/') j++;
	return j+1;
}
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
	
	
	
