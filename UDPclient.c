/**	@file UDPclient.c
 * 	@brief Contains the function implementations of creating and running a UDP client in C. 
 *	It creates a socket for sending client connections. 
 *	It is capable of sending and receiving request to and from the UDP Server. 
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */

#include "UDPclient.h" 


/*
 **************************************************
 *		FUNCTION PROTOTYPES
 **************************************************
 */
 
/**	@brief 	Is a function that takes in a error message and outputs the message to the display
*			and stops the server from running. 
*	@param 	Is the error message that should be outputted to the screen.
*	@return returns a -1 integer.
*/
int printErrorMessage( char *message );

/**	@brief Create a TCP socket and return the integer that represents the socket. 
*	@return the socket number. 
*/
int create_TCP_Socket(void);

/**	@brief 	get IP address of the server that the client will send messages to. 
*	@param 	serverName is the name of the server. 
*	@return returns a structure hostent that contains IP information about server. 
*/
struct hostent *info_Host(char * serverName);

/**	@brief Set the destination address and port of the server. 
*	@param 	hostptr is the structure containing information about the server 
*			port is the port number to connect to the server 
*			dest contains the destination IP address information. 
*	@return returns nothing.
*/
void setDestination(struct hostent *hostptr, int port, struct sockaddr_in *dest);

/*
 **************************************************
 *		CLIENT FUNCTIONS
 **************************************************
 */

/*
 **************************************************
 **************************************************
 */
int printErrorMessage( char *message ) {	
  fprintf(stderr, "ERROR: %s\n", message);
  printf("***************************************************\n\n");
  return -1;
}

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
 /*
  *	MODIFIED ON 2/6/2014
  *	REMOVED the connect function call
  */
int createSocket(char * serverName, int serverPort, struct sockaddr_in * dest) {
	int sockfd = 0;
	struct hostent *hostptr; 
	printf("***************************************************\n");
	
	//create the socket
	sockfd = create_TCP_Socket();
	if(sockfd == -1) return -1;

	//get client host information
	hostptr = info_Host(serverName);
	if(hostptr == NULL) return -1;
	
	//set server destination
	setDestination(hostptr, serverPort, dest);
	
	//return the listening socket
	return sockfd;
}


/*
 **************************************************
 *	MODIFIED ON 2/6/2014
 *	Changed SOCK_STREAM TO SOCK_DGRAM
 **************************************************
 */
int create_TCP_Socket(void){
	int sockfd = socket(AF_INET, SOCK_DGRAM,0);
	if(sockfd == -1)
		return printErrorMessage("Cannot Open Socket to Listen"); 
	struct timeval tv;
	tv.tv_sec = RECEIVE_WAIT_TIME_SEC;
	tv.tv_usec = RECEVIE_WAIT_TIME_MIL_SEC;
	//maximum wait timer
	if( setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
		return printErrorMessage("Cannot Set SO_RCVTIMEO for socket");
	return sockfd;
}

/*
 **************************************************
 **************************************************
 */
struct hostent *info_Host(char * serverName){
	struct hostent *hostptr; 
	hostptr = gethostbyname(serverName);
	if(hostptr == NULL) 
		printErrorMessage("That Host Does Not Exist");
	return hostptr;
}

/*
 **************************************************
 **************************************************
 */
void setDestination(struct hostent *hostptr, int port, struct sockaddr_in *dest){
	memset((void *) dest, 0, (size_t)sizeof(struct sockaddr_in));  
	memcpy((void *) &(dest->sin_addr), (void *) hostptr->h_addr, hostptr->h_length);
	dest->sin_family = (AF_INET);
	dest->sin_port = htons((u_short) port);
}


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
  /*
  *	MODIFIED ON 2/6/2014
  *	Print out the message being sent to the server
  *	copy the request into a full length buffer - Like DR. R TCPclient.c sendRequst function 
  */
int sendRequest(int sockFD, char * request, struct sockaddr_in * dest){
	int error = 0;
	char requestMesg[MAX_MESSAGE];
	printf("Sending the following message : \n%s\n", request);
	bzero(requestMesg, MAX_MESSAGE);
	strcpy(requestMesg, request);
	error = sendto(sockFD, requestMesg, MAX_MESSAGE, 0, (struct sockaddr *) dest, sizeof(*dest));
	if(error == -1) return printErrorMessage("Cannot Send Response to the Server");
	return 0;
}

/*
 * Receives the server's response formatted as an XML text string.
 *
 * sockfd    - the socket identifier
 * response  - the server's response as an XML formatted string to be filled in by this function into the specified string array
 * 
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
  /*
  *	MODIFIED ON 2/6/2014
  *	copy the reponse into a full length buffer - Like Dr. R TCPclient.c receiveResponse function
  *	Also added a time out command
  */
int receiveResponse(int sockFD, char * response){
	int error = 0;
	char responseMesg[MAX_MESSAGE];
	bzero(responseMesg, MAX_MESSAGE);

	error = recvfrom(sockFD, responseMesg, MAX_MESSAGE, 0, NULL, NULL);
       	if(error == -1) return printErrorMessage("Client Received No Message from Server - Time Out Occurred");
       	strcpy(response, responseMesg);
	return 0;
}

/*
 * Prints the response to the screen in a formatted way.
 *
 * response - the server's response as an XML formatted string
 *
 */
void printResponse(char * response){
	printf("Response from server : \n%s\n", response); 
	printf("***************************************************\n\n");
}

/*
 * Closes the specified socket
 *
 * sockFD - the ID of the socket to be closed
 * 
 * return - 0, if no error; otherwise, a negative number indicating the error
 */
int closeSocket(int sockFD){
	int error = close(sockFD);
	if(error == -1) return printErrorMessage("Cannot Close the Socket");
	return 0;
}

