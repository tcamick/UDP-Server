/**	@file UDPclient.h
 * 	@brief Contains the function prototypes for creating and running the UDP client that are
 *	implemented in UDPclient.c
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */
 /*
 * UDPclient.h
 *
 * This file describes the functions to be implemented by the UDPclient.
 * You may also implement any auxillary functions you deem necessary.
 */
 
 
 
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

/*
 **************************************************
 *		COMPILER PRE DEFINES
 **************************************************
 */

#define MAX_MESSAGE 256
#define RECEIVE_WAIT_TIME_SEC 1
#define RECEVIE_WAIT_TIME_MIL_SEC 0


 /*
 **************************************************
 *		FUNCTION PROTOTYPES
 **************************************************
 */
 

/*
 * Creates a datagram socket and connects to a server.
 *
 * serverName - the ip address or hostname of the server given as a string
 * serverPort - the port number of the server
 * dest       - the server's address information; the structure will be initialized with information
 *              on the server (like port, address, and family) in this function call
 *
 * return value - the socket identifier or a negative number indicating the error if a connection could not be established
 */
int createSocket(char * serverName, int serverPort, struct sockaddr_in * dest);

/*
 * Sends a request for service to the server. This is an asynchronous call to the server, 
 * so do not wait for a reply in this function.
 * 
 * sockFD  - the socket identifier
 * request - the request to be sent encoded as a string
 * dest    - the server's address information
 *
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int sendRequest(int sockFD, char * request, struct sockaddr_in * dest);

/*
 * Receives the server's response formatted as an XML text string.
 *
 * sockfd    - the socket identifier
 * response  - the server's response as an XML formatted string to be filled in by this function into the specified string array
 * 
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int receiveResponse(int sockFD, char * response);

/*
 * Prints the response to the screen in a formatted way.
 *
 * response - the server's response as an XML formatted string
 *
 */
void printResponse(char * response);

/*
 * Closes the specified socket
 *
 * sockFD - the ID of the socket to be closed
 * 
 * return - 0, if no error; otherwise, a negative number indicating the error
 */
int closeSocket(int sockFD);

