#include "encode.c"
#include "input.c"
#include "direc.c"

/*Funcoes Usaveis:
	-Encode:
		void decode(char *code);
		void start_code(char *f);
		void ins_path(char *path);
		char* encode(void);
	-Direc
		void define_root(void);
		int execute_cmd(char *input);
	-Input
		char* get_input(void);
*/

int main(void)
{
	char *s,*aloha;
	define_root();
	while(1)
	{
		s = get_input(); //cliente
		printf("Protocolo de Envio:\n\t%s\n",s);
		aloha = execute_cmd(s); //servidor
		printf("Protocolo de Retorno:\n\t%s\n", aloha);
		decode(aloha); //cliente
	}
	return 0;
}
/*
void get_patho(char path[10][300]);

int main(void)
{
	int i,tam;
	char patho[10][300];
	char folder[100];
	char *enc;
	strcpy(folder,"/A/B/C");
	start_code(folder);
	get_patho(patho);
	for(i=0; i < 10; i++)
	{
		printf("path: %s\n",patho[i]);
		ins_path(patho[i]);
	}
	enc = encode();
	printf("Code: %s\n", enc);
	decode(enc);
	return 0;
}

void get_patho(char path[10][300])
{
	strcpy(path[0],"/A/B/C/D");
	strcpy(path[1],"/A/B/C/D/F");
	strcpy(path[2],"/A/B/C/asd");
	strcpy(path[3],"/A/B/C/G/H");
	strcpy(path[4],"/A/B/C/G/H/I");
	strcpy(path[5],"/A/B/C/F/N");
	strcpy(path[6],"/A/B/C/Y");
	strcpy(path[7],"/A/B/C/T");
	strcpy(path[8],"/A/B/C/H/I/Q");
	strcpy(path[9],"/A/B/C/Y/potato");
}*/
