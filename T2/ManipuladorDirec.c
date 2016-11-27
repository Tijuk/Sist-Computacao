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
extern int alphasort();

typedef enum dir_returns
{
    D_OK = 0,
    D_MemoInsufic = 1,
    D_AcessoNegado = 2,
    D_DirecVazio = 3,
    D_DirecNaoEncontrado = 4,
    D_NotDiretorio = 5,
}D_Ret;

int remove_directory(const char *path);
int file_select(struct direct *entry);
D_Ret liststuff(char *path);
D_Ret checkpathright(char *pathname, char *findpath);
D_Ret createdirectory(char *path, int path_len, char* dirname, int dir_len);
D_Ret deletedirectoy(char *path, int path_len, char* dirname, int dir_len);
D_Ret acessdirectory(char *pathname, char *path);


int get_decision(void);


int main(void)
{
    char pathname[200];
    int a = 0;
    int decision;
    if (getcwd(pathname,200) == NULL )
    {
        printf("Error getting path\n");
         exit(0);
    }
    while(1)
    {
        decision = get_decision();
        printf("Executing decision under <<%s>>\n", pathname);
        if(decision == 1)
        {
            char new_dir[100];
            printf("Name ya new directory: ");
            scanf(" %s", new_dir);
            createdirectory(pathname,0,new_dir,strlen(new_dir));
        }
        else if(decision == 2)
        {
            char dir2del[100];
            printf("Name whatya wanna delete: ");
            scanf(" %s", dir2del);
            deletedirectoy(pathname,0,dir2del,strlen(dir2del));
        }
        else if(decision == 3)
        {
            liststuff(pathname);
        }
        else if(decision == 4)
        {
            char folder[100];
            printf("Where ya wanna go champ?: ");
            scanf(" %s", folder);
            acessdirectory(pathname, folder);
        }
        printf("\n--------------\n\n");
    }
    return 0;
}

D_Ret createdirectory(char *path, int path_len, char* dirname, int dir_len)
{
    int status;
    char pathname[200];
    char s[2] = "/";
    strcpy(pathname,path);
    strcat(pathname,s);
    strcat(pathname,dirname);
    status = mkdir(pathname,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status==0)
    {
        return D_OK;
    }
    else
    {
        return D_AcessoNegado;
    }
}

D_Ret deletedirectoy(char *path, int path_len, char* dirname, int dir_len)
{
    int status;
    if(checkpathright(path,dirname) != D_OK)
    {
        return D_DirecNaoEncontrado;
    }
    status = rmdir(dirname);
    remove_directory(dirname);
    printf("%s was deleted!!\n", dirname);
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

D_Ret liststuff(char *pathname)
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
        printf("\t%s\n", files[i-1]->d_name);
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

int get_decision(void)
{
    int cmp;
    char s[100];
    printf("What you wanna do bro?: ");
    scanf(" %s", s);
    cmp = strcmp(s,"CREATE");
    if(cmp == 0) return 1;
    cmp = strcmp(s,"DELETE");
    if(cmp == 0) return 2;
    cmp = strcmp(s,"LIST");
    if(cmp == 0) return 3;
    cmp = strcmp(s,"ACESS");
    if(cmp == 0) return 4;
    return 0;
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

D_Ret acessdirectory(char *pathname, char *path)
{
    DIR *dir;
    int count,i;
    struct dirent *ent;
    struct direct **files;
    struct stat status;
    int filer = 0;
    int file_select();
    char s[3] = "/";
    strcat(pathname,s);
    strcat(pathname,path);
    count = scandir( pathname, &files, file_select, alphasort);
    if (!(dir = opendir(pathname)))
    {
        return 0;
    }
    printf("Number of files = %d\n",count);
    return 0;
}


int opendirec(char* pathname, char* npath)
{
    DIR *dir;
    int count,i;
    struct dirent *ent;
    struct direct **files;
    struct stat status;
    int filer = 0;
    int file_select();
    char s[3] = "/";
    if(npath[0] != '\0')
    {
        strcat(pathname,s);
        strcat(pathname,npath);
    }
    count = scandir( pathname, &files, file_select, alphasort);
    if (!(dir = opendir(pathname)))
    {
        return 0;
    }
    printf("Number of files = %d\n",count);
    return 0;
}
