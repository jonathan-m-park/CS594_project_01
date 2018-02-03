# CS594_project_01

# This project conatains the implementation of a simple file transfer protocol for use in CS594 Intro to Internetworking Protocols at PSU.
# Data transfer utilizes UDP sockets and datagrams. TCP-like features - including reliable data transfer, flow control, and retransmission - 
# are implemented within the client-side and server-side applications.

#
#
# The basic description of the protocol is that is allows a user to connect a client and server, select a file via an absolute path name,
# and transfer the file. Example commands are included below:

#     	       	   	 	 	      	       ./CS594rcp_server  1080

# The user must first start the server and designate a port to listen to connections for clients. In the example above the sever is listing
# on port 1080.

#				./CS594rcp_client 120.45.67.126 1080 /Documents/Files/My_File.txt

# The user command-line intructions for the client-side allows the user to specify the IP address, port number of the server, as well as the
# path for the file. In the example above, the client requests to connect to the server specified by address 120.45.67.126, through port
# 1080, and copy the file specified by the file path /Documents/Files/My_File.txt.

