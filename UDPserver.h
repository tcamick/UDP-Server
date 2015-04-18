/**	@file UDPserver.h
 * 	@brief Contains the function prototypes for creating and running the UDP server that are
 *	implemented in UDPserver.c
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
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
 
#define INTERFACE "eth0"
#define MAX_MESSAGE 256
#define IP_4 32
#define ECHO_XML_START 6
#define ECHO_XML_END 7
#define LOADAVG_XML 10
#define NEW_LINE 1
#define LOAD_AVG_FUNCTION 3
#define LOAD_AVG_1_MIN_INDEX 0
#define LOAD_AVG_5_MIN_INDEX 1
#define LOAD_AVG_15_MIN_INDEX 2

/*
 **************************************************
 *		FUNCTION PROTOTYPES
 **************************************************
 */
 
/**	@brief	Function create a UDP socket by calling the "socket" function.
*	@param 	no parameter is passed. 
*	@return a integer representing the socket number.
*/
int create_UDP_Socket(void);

/**	@brief 	Get the host that the server is running on and returns it in the stuct hostent.
*	@param 	no parameter is passed. 
*	@return returns a pointer to a hostent structure containing information about the server's host.
*/
struct hostent *info_Host();

/**	@brief 	Set the program host address and port number to be able to connect the server.
*	@param 	hostptr is a pointer to the hostent structure that contains the server host information.
*   @param  SERVER_PORT is the port number
*	@return return a sockaddr_in structure that can be used to link the server program to the host computer. 
*/
struct sockaddr_in destination_Address( struct hostent *hostptr, int SERVER_PORT);

/**	@brief 	binds the socket with the host that will run the server program. 
*	@param 	listensockfd is the socket that the server will listen on. 
*			servaddr is a sockaddr_in structure that contains information about the host running the server. 
*	@return returns a sockaddr_in structure that can be used to link the server program to the host computer. 
*/
struct sockaddr_in bind_Socket(int listensockfd, struct sockaddr_in servaddr);

/**	@brief 	Prints the host name, IP address, and port number that the server is running on. 
*	@param 	listensockfd is the socket that the server will listen on. 
*			hostptr contains information about the host the server is running on.
*			servaddr is a sockaddr_in structure that contains information about the host running the server. 
*	@return returns nothing. 
*/
void print_Server_info(int listensockfd, struct hostent *hostptr, struct sockaddr_in servaddr);

/**	@brief 	Function to accept connections and wait if the server is full of request. 
*	@param 	sockfd is the socket that the server will listen on. 
*   @return returns nothing.
*/
void run_Server(int sockfd);

