#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********************\
*	FUNCOES DO CLIENTE	*
\***********************/
void decode(char *code);


/***********************\
*	FUNCOES DO SERVER	*
\***********************/

void start_code(char *f);
/*
start_code = inicia o encode(inicia as variaveis)
deve ser chamada toda vez que quiser iniciar um NOVO encode
*/
void ins_path(char *path);
/*
ins_path, joga o path(JA CORTADO) dentro de uma pilha(vetor)
*/

char* encode(void);
/*
retorna a string codificada
*/

