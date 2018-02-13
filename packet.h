/******************************************************************************/
/*                                                                            */
/* Author:      Jonathan M. Park                                              */
/* Date:        02/11/2018                                                    */
/* Descruption: Packet header file to contain basic structs, macros, and      */
/*              functions for handling and parsing packets.                   */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>

/* The maximum size of the datagram string. The current value is              */
/* 512 bytes plus one element for the flag 'type', one for the seq element,   */
/* one for the length of data, and one for the end of the array.              */
#define MAX_DGRAM_LEN 516
#define MAX_DATA_LEN  512

typedef enum dg_type {DG_ACK, DG_DATA, DG_HELLO, DG_CLOSE} type;
char         dg_type_arry[5] = {'0', '1', '2', '3'};
