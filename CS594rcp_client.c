/******************************************************************************/
/*      								      */
/* Author:      Jonathan M. Park                                              */
/* Date:        02/10/2018                                                    */
/* Descruption: A                                                             */
/*									      */
/*								 	      */
/******************************************************************************/

#include <stdio.h>      /* for standard i/o  */
#include <sys/types.h>  /* for sizes  */
#include <sys/socket.h> /* for socket functions  */
#include <arpa/inet.h>  /* for inet_addr()  */
#include <stdlib.h>     /* for atoi()  */
#include <string.h>     /* for memset() and strcpy()  */
#include <unistd.h>     /* for close()  */

#define BUFFERSIZE 32

void err(char *);

int
main(int argc, char **argv)
{
  int port;                     /* The port_no as specificed by the user.  */
  int rc;                       /* Return code for error handling.  */
  int serv_sock;                /* Socket descriptorr for the server.  */
  int serv_addr_size;           /* size of server address for recvfrom()  */
  char *serv_ip;                /* IP address of the server.  */
  char buf[BUFFERSIZE];         /* Temporary buffer for message testing.  */
  char res[BUFFERSIZE];         /* Temporary buffer for response testing.  */
  struct sockaddr_in serv_addr; /* Socket API defined form for addresses  */

  /* argv[1] is the the IP address of the server.
     argv[2] is the port number of the server.
     argv[3] is the file path for the desired file.  */
  if(argc < 4 || argc > 5)
    err("Usage: should be ./CS594rcp_client IP_addr port_no file_path.\n");

  serv_ip = argv[1];
  port = atoi(argv[2]);

  /* Create a  UDP socket.  */
  serv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(serv_sock < 0)
    err("Socket not created.\n");

  /* bzero() is used to zero serv_addr struct and then fields are filled with 
     appropriate values  */
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET; /* Interner domain.  */
  serv_addr.sin_port = htons(port); /* Server port.  */
  /* htons() used to solve endianness incompatibility across machines.  */
  serv_addr.sin_addr.s_addr = inet_addr(serv_ip); /* Server's address.  */

  strcpy(buf, "Hello.\n"); /* Simple buffer used for testing.  */
  memset(&res, 0, sizeof(res)); /* Fill the response buffer with zeros.  */

  serv_addr_size = sizeof(serv_addr);

  /* Send the buffer message to the server.  */
  rc = sendto(serv_sock, buf, (strlen(buf) + 1), 0, 
		  (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if(rc < 0)
    err("sendto() failed.\n");

  /* receive message from server  */
  rc = recvfrom(serv_sock, res, sizeof(res), 0, (struct sockaddr *) &serv_addr,
                (socklen_t *) &serv_addr_size);
  if(rc < 0)
    err("recvfrom() failed.\n");

  printf("Message Received from server: %s", res);

  close(serv_sock); /* Close server connection  */

  return 0;
}

/* error handling function. It receives a char* msg which states the issue. 
   The message is printed with perror() and the function then exits with 
   error (exit(1)). */
void
err(char *msg)
{
  perror(msg);
  exit(0);
}

