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
#include "packet.h" 

void serv_handshake(int, struct sockaddr *, socklen_t);
void dg_handler(int, struct sockaddr *, socklen_t, char *);
void close_cli(int, struct sockaddr *, socklen_t);

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

  while(1){
      serv_handshake(cli_sock, (struct sockaddr *) &cli_addr, sizeof(cli_addr));
  }
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
dg_handler(int sockfd, struct sockaddr * pcli_addr, 
	   socklen_t clilen, char *pathname){
    int rc;
    int i;
    int num_dgs;
    socklen_t len = clilen;
    /* the following is a test.  */
    FILE *file;
    char **datagrams;
    char res[MAX_DGRAM_LEN];
    unsigned long fileLen;

    printf("Requested file: %s\n", pathname);
	//Open file
    file = fopen(pathname, "rb");
    if (!file){
	fprintf(stderr, "Unable to open file %s", pathname);
	return;
    }
	
	//Get file length
    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    fseek(file, 0, SEEK_SET);

    num_dgs = (fileLen / MAX_DATA_LEN);
    if (fileLen % MAX_DATA_LEN)
      num_dgs += 1;

	//Allocate memory
    datagrams = (char **) malloc (num_dgs * (MAX_DGRAM_LEN));
    if (!datagrams){
	fprintf(stderr, "Memory error!");                        
	fclose(file);
	return;
    }

    for (i = 0; i < num_dgs; i++){
      datagrams[i] = (char *) malloc (MAX_DGRAM_LEN);
      if (!datagrams[i]){
	printf("Memory Error!");
	fclose(file);
	return;
      }
    }
    
    //Read file contents into buffer
    for (i = 0; i < num_dgs; i++){
      fread((datagrams[i]) + 2, MAX_DATA_LEN, 1, file /*+ (MAX_DATA_LEN * i)*/);
    }
    fclose(file);
    for (i = 0; i <num_dgs; i++){
      (datagrams[i])[0] = dg_type_arry[DG_DATA];
      (datagrams[i])[1] = (i%2)+'0';
      printf("%s\n", datagrams[i]);
      rc = sendto(sockfd, datagrams[i], MAX_DGRAM_LEN + 1, 0, pcli_addr, len);
      if (rc < 0)
	perror("sendto() failed. Will try again.");
      printf("Number of bytes sent: %d\n", rc);
      /*rc = recvfrom(sockfd, res, MAX_DGRAM_LEN + 1, 0, pcli_addr, &len);
      if (rc < 0)
	perror("Did not receive ACK\n");
      if (res[0] == dg_type_arry[DG_ACK])
	close_cli(sockfd, pcli_addr, len);*/
    }
    close_cli(sockfd, pcli_addr, len);
    for (i = 0; datagrams[i]; i++){
      free(datagrams[i]);
    }
    free(datagrams);
}

void
serv_handshake(int sockfd, struct sockaddr *paddr, socklen_t addr_len)
{
    int n;
    char msg1[MAX_DGRAM_LEN];
    char msg2[MAX_DGRAM_LEN];
    char pathname[MAX_DATA_LEN];
    int handshake = 1;

    msg2[0] = dg_type_arry[DG_HELLO];
    do{
    n = recvfrom(sockfd, msg1, MAX_DGRAM_LEN, 0, paddr, &addr_len);
    if(n < 0){
	perror("No handshake receieved.\n");
        //break;
    }
    if(msg1[0] != dg_type_arry[DG_HELLO]){
	perror("Handshake not established yet.\n");
	//break;
    }
    else{
	n = sendto(sockfd, msg2, MAX_DGRAM_LEN, 0, paddr, addr_len);
	if(n < 0){
	    perror("Unable to send handshake.\n");
	    //break;
	}
	bzero(msg1, sizeof(msg1));
	n = recvfrom(sockfd, msg1, MAX_DGRAM_LEN, 0, paddr, &addr_len);
	if(n < 0){
	    perror("No path name sent.\n");
	    //break;
	}
	strncpy(pathname, msg1 + 3, sizeof(pathname)); /* + 3 to skip header.  */
	handshake = 0;
	}
    }while(handshake);

    dg_handler(sockfd, paddr, addr_len, pathname);
}

void
close_cli(int sockfd, struct sockaddr * paddr, socklen_t addr_len)
{
    int rc;
    char close_msg[MAX_DGRAM_LEN];
    close_msg[0] = dg_type_arry[DG_CLOSE];

    rc = sendto(sockfd, close_msg, MAX_DGRAM_LEN, 0, paddr, addr_len);
    if(rc < 0)
	perror("Unable to send close message.\n");
}
