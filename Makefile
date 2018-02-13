CC=gcc
CFLAGS=-Wall -g

.PHONY: all

all: CS594rcp_client CS594rcp_server

CS594rcp_cilent: CS594rcp_client.o
	${CC} ${CFLAGS} CS594rcp_client.o -o CS594rcp_client

CS594rcp_client.o: CS594rcp_client.c packet.h
	${CC} ${CFLAGS} -c CS594rcp_client.c -o CS594rcp_client.o

CS594rcp_server: CS594rcp_server.o
	${CC} ${CFLAGS} CS594rcp_server.o -o CS594rcp_server

CS594rcp_server.o: CS594rcp_server.c packet.h
	${CC} ${CFLAGS} -c CS594rcp_server.c -o CS594rcp_server.o

tar:
	tar -cvf project.tar.gz CS594rcp_client.c CS594rcp_server.c Makefile packet.h
