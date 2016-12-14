#pragma once

#define _GNU_SOURCE
#define CMAX 3000
#define MAX_PATH_SIZE 3000
#define MAX_SIZE 1000
#define ROOT_PATH ""
#define FULL_ROOT "SFS-root-di"
#define MAXARG 10
#define MP 300
#define MPATH 100

#define DEF_RD 0 //read file
#define DEF_WR 1 //write file
#define DEF_FI 2 //find info
#define DEF_DC 3 //direc create
#define DEF_DR 4 //direc remove
#define DEF_DL 5 //direc list all
#define DEF_OL 6 //direc list directory


/*
RD retorna oq leu em payload
	RD-REP,path,strlen, payload, nrbytes lidos, offset(igual ao do RD-REQ)
WR retorna confirmacao da operacao
	WR-REP,path,strlen, payload(string vazio), nrbytes escritos, offset(igual ao do WR-REQ)
	-nb > 0 == confirmacao
FI retorna cliente dono e seu tamanho
	FI-REP, path, strlen, owner(int), permissions(2char), filelength
DC retorna se operacao foi realizada
	-strlen = 0 se operacao nao pode ser realizada
DR retorna o novo path ja sem dirname, ou strlen = 0
	-strlen = 0 se operacao nao pode ser realizada
DL retorna encode
*/
