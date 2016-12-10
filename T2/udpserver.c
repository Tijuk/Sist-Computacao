/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "auxiliar.h"
#include "encoder.c"

#define BUFSIZE 1024

extern int alphasort();
int clienteID;
char root_path[MP];

void error(char *msg) {
  perror(msg);
  exit(1);
}/*
int parse_buff(char *buff, int *cmd, char *name) {
    char *cmdstr;    
    cmdstr = strtok(buff," ");
    name = strtok(NULL,"\0");
    cmd = atoi(cmdstr);
}*/

int main(int argc, char **argv) {

  /****** Parte Servidor SFMP ***********/
  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
    
  char name[BUFSIZE];   // name of the file received from client
  int cmd;              // cmd received from client

  /******* Parte Servidor SFMP *********/

  /******* Parte Interpretador e Decision ********/

    	Info *a;
	a = (Info*)malloc(sizeof(Info));
	int fd,ret;
	char pathname[MP];
	clienteID = 0;
	char lpath[MP];

  /******* Parte Interpretador e Decision ********/


  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
	   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1) {
    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);


    if (n < 0)
      error("ERROR in recvfrom");

    //parse_buff(buf, &cmd, name);//Esse fucking parser tira da string qualquer coisa depois do primeiro " ". A mensagem do cliente então deve ser escrita sem espacos, tudo em sequencia, como no enunciado.

/*
É nessa area do codigo que vamos mexer no servidor, e onde devemos chamar 
as funçõe auxiliares para tratar o comando enviado pelo cliente.					 
*/		
/**********************************************************************/
	
	// Pegar o root 
	strcpy(root_path,pathname);

	
	// Pega a decisao da string	
	get_decision(a,buf);
	// Teste dos parametros
	exibe_code(a);
	
	sprintf(lpath,"%s%s", pathname, a->path);
	/*Vou criar uma funcao auxiliar para interpretar o comando lido*/
	get_command(a,lpath);

/***********************************************************************/


    /* 
     * gethostbyaddr: determine who sent the datagram
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server received datagram from %s (%s)\n", 
	   hostp->h_name, hostaddrp);
    printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);
    
    /* 
     * sendto: echo the input back to the client 
     */

	char * aux1; 

    n = sendto(sockfd, buf, strlen(buf), 0, 
	       (struct sockaddr *) &clientaddr, clientlen);
 
    if (n < 0) 
      error("ERROR in sendto");
  }
}
