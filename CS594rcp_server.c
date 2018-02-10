/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>      /* for printf() and similar functions  */
#include <unistd.h>     /* for close()  */
#include <string.h>     /* for bzero() and strcoy()  */
#include <arpa/inet.h>  /* for INADDR_ANY  */
#include <sys/socket.h> /* for socket functions   */
#include <sys/types.h>   /* for socketlen_t  */
#include <stdlib.h>

#define MAXBUFFERSIZE 256

void dg_handler(int, struct sockaddr *, socklen_t);

int 
main(int argc, char **argv){
  int cli_sock, port, rc;
  struct sockaddr_in serv_addr, cli_addr;

  if(argc < 2 || argc > 3){
      perror("Incorrect USAGE. Expected ./CS594rcp_serv port_no.\n");
      exit(1);
  }
  port = atoi(argv[1]);

  cli_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(cli_sock < 0)
      perror("cli_socket not created.\n");

  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  rc = bind(cli_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if(rc < 0){
      perror("bind() failed. Please restart server.\n");
      exit(1);
  }

  dg_handler(cli_sock, (struct sockaddr *) &cli_addr, sizeof(cli_addr));

  return 0;
}

void
dg_handler(int sockfd, struct sockaddr * pcli_addr, socklen_t clilen){
    int n, rc;
    socklen_t len;
    char msg[MAXBUFFERSIZE];
    char res[MAXBUFFERSIZE] = "Hello.\n";

    while(1){
	len = clilen;
	n = recvfrom(sockfd, msg, MAXBUFFERSIZE, 0, pcli_addr, &len);
	if(n < 0)
	    perror("recvfrom() failed. Please try again.\n");

	rc = sendto(sockfd, res, sizeof(res), 0, pcli_addr, len);
	if(rc < 0)
	    perror("sendto() failed. Will try again.\n");
    }
}


