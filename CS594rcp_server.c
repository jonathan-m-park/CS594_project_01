/******************************************************************************/
/*                                                                            */
/* Author:      Jonathan M. Park                                              */
/* Date:        02/10/2018                                                    */
/* Description: Simple call-and-response server for testing a simple,         */
/*              reliable file transfer protocol with UDP datagrams. The       */
/*              will be multi-threaded to handle multiple client connections. */
/*              The protocol includes retransmission and timeout for          */
/*              discarded datagrams                                           */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>      /* for printf() and similar functions  */
#include <unistd.h>     /* for close()  */
#include <string.h>     /* for bzero() and strcoy()  */
#include <arpa/inet.h>  /* for INADDR_ANY  */
#include <sys/socket.h> /* for socket functions   */
#include <sys/types.h>  /* for socketlen_t  */
#include <stdlib.h>     /* for exit()  */

#define MAXBUFFERSIZE 256

void dg_handler(int, struct sockaddr *, socklen_t);

int 
main(int argc, char **argv){
  int cli_sock, port, rc;
  struct sockaddr_in serv_addr, cli_addr;

  /* Testing for invalid command-line usage. The 1th argument should be the 
   * port number.  */
  if(argc < 2 || argc > 3){
      perror("Incorrect USAGE. Expected ./CS594rcp_serv port_no.\n");
      exit(1);
  }
  port = atoi(argv[1]); /* converting the port number argument to an int.  */

  /* Create and error check a UDP socket.  */
  cli_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(cli_sock < 0)
      perror("cli_socket not created.\n");

  /* Zero the server address data structure and fill in the fields.  */
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port); /* htons() used to handle endianess.  */
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  /* INADDR_ANY used to accept traffic from any IP address.  */

  /* The newly created socket is bound to the server address and the return 
   * code is tested for error.  */
  rc = bind(cli_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if(rc < 0){
      perror("bind() failed. Please restart server.\n");
      exit(1);
  }

  /* The dg_handler() function is used to perform the server's work.   */
  dg_handler(cli_sock, (struct sockaddr *) &cli_addr, sizeof(cli_addr));

  return 0;
}

/* The function handles datagrams sent to - and received from the client.  
 * For testing, the function simply spins forever and receieves messages 
 * and replies with "Hello".  
 *
 * No value is returned and the server continues to run indefinitely or 
 * until it is shutdown. 
 *
 * The parameters are straightforward. A socket descriptor, the client
 * address data structure to send and receive data, and the length of 
 * the client address structure.  */
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


