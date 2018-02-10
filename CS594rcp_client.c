////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////

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
  int port;                     /* int of the port_no as specificed by the user  */
  int rc;                       /* return code for error handling  */
  int serv_sock;                /* socket identifier for the user specified server   */
  int serv_addr_size;           /* size of server address for recvfrom()  */
  char *serv_ip;                /* IP address of the server specified by the user  */
  char buf[BUFFERSIZE];         /* temporary buffer for message testing  */
  char res[BUFFERSIZE];         /* temporary buffer for response testing  */
  struct sockaddr_in serv_addr; /* socket API defined form for addresses  */

  /* argv[1] is the the IP address of the server.
     argv[2] is the port number of the server.
     argv[3] is the file path for the desired file.  */
  if(argc < 4 || argc > 5)
    err("Invalid Usage: should be ./CS594rcp_client IP_addr port_no file_path.\n");

  serv_ip = argv[1];
  port = atoi(argv[2]);

  /* Create UDP socket  */
  serv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(serv_sock < 0)
    err("Socket not created.\n");

  /* memset to zero serv_addr struct and then fields are filled with 
     appropriate values  */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET; /* Interner Domain  */
  serv_addr.sin_port = htons(port); /* Server Port  */
  /* htons() used to solve endianness incompatibility across machines */
  serv_addr.sin_addr.s_addr = inet_addr(serv_ip); /* Server's Address  */

  strcpy(buf, "Hello.\n"); /* buffer used for testing  */
  memset(&res, 0, sizeof(res)); /* fill response buffer with zero  */

  serv_addr_size = sizeof(serv_addr);

  /* send buffer message to server  */
  rc = sendto(serv_sock, buf, (strlen(buf) + 1), 0, (struct sockaddr *) &serv_addr,
     sizeof(serv_addr));
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

