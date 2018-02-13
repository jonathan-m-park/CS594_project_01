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
#include "packet.h"     

void cli_handshake(int, struct sockaddr *, socklen_t, char *);
void write_file(char *, char *);
void err(char *);

int
main(int argc, char **argv)
{
  int port;                     /* The port_no as specificed by the user.  */
  int rc;                       /* Return code for error handling.  */
  int serv_sock;                /* Socket descriptorr for the server.  */
  int serv_addr_size;           /* size of server address for recvfrom()  */
  char *serv_ip;                /* IP address of the server.  */
  /* char buf[MAX_DGRAM_LEN];      * Temporary buffer for message testing.  */
  char res[MAX_DGRAM_LEN];      /* Temporary buffer for response testing.  */
  char ack_msg[MAX_DGRAM_LEN];
  struct sockaddr_in serv_addr; /* Socket API defined form for addresses  */
  char last_seq = '1';

  /* argv[1] is the the IP address of the server.
     argv[2] is the port number of the server.
     argv[3] is the file path for the desired file.  */
  if(argc < 4 || argc > 5)
    err("Usage: should be ./CS594rcp_client IP_addr port_no file_path.\n");

  serv_ip = argv[1];
  port = atoi(argv[2]);

  /* testing.  */
  FILE *f;

  f = fopen("myfile.txt", "ab"); /* removed wb for testing */
  if(!f)
    err("File could not be made.\n");

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

  memset(&res, 0, sizeof(res)); /* Fill the response buffer with zeros.  */

  serv_addr_size = sizeof(serv_addr);

  cli_handshake(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr), argv[3]);  

  do{
    bzero(&ack_msg, sizeof(ack_msg));
    ack_msg[0] = dg_type_arry[DG_ACK];
    memset(&res, 0, sizeof(res)); /* Fill the response buffer with zeros.  */
    /* receive message from server  */
    rc = recvfrom(serv_sock, res, MAX_DGRAM_LEN + 1, 0, (struct sockaddr *) &serv_addr,
                (socklen_t *) &serv_addr_size);

    if(rc < 0){
      err("recvfrom() failed.\n");
      break;
    }
    if(res[0] == dg_type_arry[DG_CLOSE])
      break;
    /*else if(res[1] == last_seq){ //if duplicate
      printf("Duplicate PKT, resending ACK%c\n", last_seq);
      ack_msg[1] = last_seq;
      rc = sendto(serv_sock, ack_msg, MAX_DGRAM_LEN, 0, (struct sockaddr *) &serv_addr,
	      	  (socklen_t) serv_addr_size);
    }*/
    else{
      printf("SEQ:%d\n", res[1]);
      if (res[1] != last_seq)
        fwrite(res + 2, 1, rc, f);

      ack_msg[1] = res[1];
      last_seq = res[1];

      rc = sendto(serv_sock, ack_msg, MAX_DGRAM_LEN, 0, (struct sockaddr *) &serv_addr,
		  (socklen_t) serv_addr_size);
      if(rc < 0)
        err("sendto() failed.\n");
    }
  } while(res[0] != dg_type_arry[DG_CLOSE]);

  // if(res[0] == dg_type_arry[DG_CLOSE])
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

void
write_file(char *name, char *buf)
{
    FILE *f;
    
    f = fopen(name, "wb");
    if(!f)
	err("File could not be made.\n");

    printf("%lu\n", sizeof(buf));
    fwrite(buf, 1, sizeof(buf), f);

    fclose(f);
}

void
cli_handshake(int sockfd, struct sockaddr * paddr, socklen_t servlen, char *pathname)
{
    int rc;
    char msg1[MAX_DGRAM_LEN];
    char msg2[MAX_DGRAM_LEN];
    char msg3[MAX_DGRAM_LEN];
    socklen_t len = servlen;

    msg1[0] = dg_type_arry[DG_HELLO];
    rc = sendto(sockfd, msg1, MAX_DGRAM_LEN, 0, paddr, len);
    if(rc < 0)
	err("Unable to start handshake.\n");
    rc = recvfrom(sockfd, msg2, MAX_DGRAM_LEN, 0, paddr, &len);
    if(rc < 0)
	err("Unable to establish handshake.\n");
    do{
	msg3[0] = dg_type_arry[DG_DATA];
	msg3[1] = '0';
	msg3[2] = '1';
	strncpy(msg3 + 3, pathname, strlen(pathname));

	rc = sendto(sockfd, msg3, MAX_DGRAM_LEN, 0, paddr, len);
	if(rc < 0)
	    printf("Unable to send pathname to server. Will try again.\n");
    } while(rc < 0);
}

