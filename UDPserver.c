/**	@file UDPserver.c
 * 	@brief Contains the function implementations of creating and running a UDP server.
 *	It creates a socket, binds the socket and waits for incoming connections
 *	It automatically determines the local host information that the server is running on.
 *	The port must be supplied by the user. 
 *	This information can be outputted to the screen to allow clients to connect. 
 *	Messages can be sent to the server in the following format:
 *	<echo>message</echo>
 *	<loadavg/>
 *	<shutdown/>
 *	If a message is sent that is not in the above format, 
 *	server responses with <error>unknown format</error>.
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */
 
#include "UDPserver.h"

/*
 **************************************************
 *		FUNCTION PROTOTYPES
 **************************************************
 */
 
/**	@brief 	Is a function that takes in a error message and outputs the message to the display
*			and stops the server from running. 
*	@param 	Is the error message that should be outputted to the screen.
*	@return returns nothing. 
*/
void printErrorMessage( char *message );


/**	@brief 	Modifies the sent message and return the modified message to the client. 
*	@param 	sockfd is the sock that the server is using for communation. 
*			cliaddr is a structure containing the connected client identification
*			recvMesg is a char array containing the client message that was sent to the server. 
*	@return returns a 1 if the shutdown command has been given else returns 0.
*/
int handleMessage(int sockfd, struct sockaddr_in cliaddr, char *recvMesg);


/**	@brief 	Determines if the message is a valid ECHO or LOADAVG command or
*			if the message is a error message, and makes decisions based upon this.
*	@param 	*recvMesg is a char array containing the client message that was sent to the server.
*			*send is the char array representing the message to be sent back to the client. 
*	@return returns a 1 if the shutdown command has been given else returns 0.
*/
int modifyMessage(char *recvMesg, char *send);


/**	@brief 	The client sent a message in the ECHO header and should be returned to the client
*			in REPLY headers. 
*	@param 	*recvMesg is a char array containing the client message that was sent to the server. 
*			*send is the char array representing the message to be sent back to the client. 
*	@return returns nothing. 
*/
void echoMessage(char *recvMesg, char *send);


/**	@brief 	The client sent the <loadavg/> message and therefore the load average
*			on the server for 1:5:15 minutes.
*	@param 	*recvMesg is a char array containing the keyword -> <loadavg/>
*			*send is a char array containing the load average calculations. 
*	@return returns nothing. 
*/
void loadavgMessage(char *recvMesg, char *send);


/**	@brief	The client sent the server a invalid message and must be returned
*			to the client as a invalid input. 
*	@param 	*recvMesg is a char array contains the message that the client sent to the server.
*			*send is the char array representing the message to be sent back to the client. 
*	@return	returns nothing. 
*/
void errorMessage(char *recvMesg, char *send);


/**	@brief	Used to reverse a string pass into the function. 
*	@param	*original is the string to reverse, it directly modifies this char array 
*	@return	returns nothing. 
*/
void reverseString(char *original);


/*
 **************************************************
 *		SERVER FUNCTIONS
 **************************************************
 */
 
/*
 **************************************************
 **************************************************
 */
void printErrorMessage( char *message ) {	
  fprintf(stderr, "ERROR: %s\n", message);
  exit(1);
}


/*
 **************************************************
 *	MODIFIED ON 2/6/2014
 *	Changed to SOCK_DGRAM
 **************************************************
 */
int create_UDP_Socket(void){
  int sockfd = socket(AF_INET, SOCK_DGRAM,0);
  if(sockfd == -1)
	printErrorMessage("Cannot Open Socket to Listen"); 
  return sockfd;
}


/*
 **************************************************
 **************************************************
 */
struct hostent *info_Host(void) {
  char hostname[IP_4];
  struct hostent *hostptr; 
  if(gethostname(hostname, IP_4) == -1) 
	printErrorMessage("Cannot Get The Host Name");
  hostptr = gethostbyname(hostname);
  if(hostptr == NULL) 
	printErrorMessage("That Host Does Not Exist");
  return hostptr;
}


/*
 **************************************************
 *	MODIFIED ON 2/6/2014
 *	Lien 144: Server port is set from user input given to function as parameter
 **************************************************
 */
struct sockaddr_in destination_Address( struct hostent *hostptr, int SERVER_PORT) {
  struct sockaddr_in servaddr;
  memset((void *) &servaddr, 0, (size_t) sizeof(servaddr));
  memcpy((void *) &servaddr.sin_addr, (void *) hostptr->h_addr, hostptr->h_length);
  servaddr.sin_family = (AF_INET);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERVER_PORT);
  return servaddr;
}


/*
 **************************************************
 **************************************************
 */
struct sockaddr_in bind_Socket(int sockfd, struct sockaddr_in servaddr){
  if(bind(sockfd, (struct sockaddr *) &servaddr, (socklen_t) sizeof(servaddr)) == -1)
	printErrorMessage("Failed to Bind To Socket");  
  return servaddr;
}

/*
 **************************************************
 **************************************************
 */
void print_Server_info(int sockfd, struct hostent *hostptr, struct sockaddr_in servaddr){
  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, INTERFACE, IFNAMSIZ-1);
  if(ioctl(sockfd, SIOCGIFADDR, &ifr) == -1)
	printErrorMessage("Cannot Get Server IP Address");
  
  printf("\nHostname Name : %s\n", hostptr->h_name);
  printf("Host IP Address : %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  printf("Host Port Number : %i\n\n", htons(servaddr.sin_port));
}


/*
 **************************************************
 *	MODIFIED ON 2/6/2014
 *	Modified to wait for new client connections on UDP 
 *	REMOVED "accept", "pthread_create", "pthreada_detach", "pthread_exit.
 *	Combined this function with the original "run_Server" and "receiveMessage" function from the TCPserver program since we do not have threads
 *	Used Bzero rather than memset
 **************************************************
 */

void run_Server(int sockfd){
  char recvMesg[MAX_MESSAGE];
  int shutdown = 0;
  struct sockaddr_in cliaddr; //used for storing client information
  socklen_t clilen = sizeof(cliaddr);
  
  //continue receiving from the currently connected client 
  while(!shutdown) 
  {
      bzero(recvMesg, MAX_MESSAGE);
      printf("Waiting for Connection ......\n");
      fflush(stdout);
      
      //receive message from client 
      recvfrom(sockfd, recvMesg, MAX_MESSAGE, 0,(struct sockaddr *) &cliaddr, &clilen);
      
      shutdown = handleMessage(sockfd, cliaddr, recvMesg);
  }
  close(sockfd);
}




/*
 **************************************************
 *	MODIFIED ON 2/6/2014
 * 	Original Function : void handleMessage(ClientStruct_P clientStruct_p, char *recvMesg){
 *	Modified Function:	int handleMessage(int sockfd, struct sockaddr_in cliaddr, char *recvMesg) {
 *  pass the socket id and client strcture to the function instead of putting into a structure as we did on TCP
 * 	Put if-else for handling sending messages to the client only when the shutdown command is not given. 
 *	Returns a 1 if the shutdown command is given.
 *	Line 249: Modified to print a message when the shutdown command is given and do not send a message to the client. 
 **************************************************
 */
int handleMessage(int sockfd, struct sockaddr_in cliaddr, char *recvMesg){
  int shutdown = 0; 
  char sendMesg[MAX_MESSAGE];
  socklen_t clilen = sizeof(cliaddr);
  bzero(sendMesg, MAX_MESSAGE);
  
  //remove newline character at ending if present
  if(recvMesg[ ( strlen(recvMesg) - NEW_LINE ) ]  == '\n') 
	recvMesg[ ( strlen(recvMesg) - NEW_LINE ) ] = '\0';
  
  //print client message
  printf("***************************************************\n");
  printf("Received the following message from : %s\n%s\n", inet_ntoa(((struct sockaddr_in *) &(cliaddr))->sin_addr), recvMesg);
  
  //modify the incoming message 
  shutdown = modifyMessage(recvMesg, sendMesg);

  //send the client the modified message
  sendto(sockfd, sendMesg, MAX_MESSAGE, 0, (struct sockaddr *) &cliaddr, clilen);
  printf("Sent the following message to : %s\n%s", inet_ntoa(((struct sockaddr_in *) &(cliaddr))->sin_addr), sendMesg);
  printf("\n***************************************************\n\n");
  
  //do shutdown
  if(shutdown == -1) {
    printf("\n***************************************************\n");
    printf("The Server Is Being Powered OFF !!");
    printf("\n***************************************************\n\n");
    return -1;
  }
  return 0;
}


/*
 **************************************************
 *	MODIFIED ON 2/6/2014
 *	Line 266: Change to use case sensitiive string compares
 *	Line 272: Include code for handling the shutdown command
 *	returns a -1 when the shutdown command is given. 
 **************************************************
 */
int modifyMessage(char *recvMesg, char *send){
  char *sendMesg = send;
  //handle <echo> messages
  if(!strncasecmp(recvMesg, "<echo>", ECHO_XML_START))
	echoMessage(recvMesg, sendMesg); 
  //handle <loadavg/> messages
  else if(!strcasecmp(recvMesg, "<loadavg/>"))
   	loadavgMessage(recvMesg, sendMesg); 
  //handle <shutdown/> messages
  else if(!strcasecmp(recvMesg, "<shutdown/>")){
    strcpy(sendMesg, "<replyShutDown>Server is shutting down</replyShutDown>");
    return -1;
  }
  //handle error messages
  else
	errorMessage(recvMesg, sendMesg); 
  return 0;
}


/*
 **************************************************
 *	MODIFIED ON 2/6/2014
 *	Line 289: Changed MEMSET of reverseMesg and modifiedReceiveMessage to bzero
 **************************************************
 */
void echoMessage(char *recvMesg, char *send){
  char *sendMesg = send, reverseMesg[MAX_MESSAGE], modifiedReceiveMessage[MAX_MESSAGE];
  bzero(reverseMesg, MAX_MESSAGE);
  bzero(modifiedReceiveMessage, MAX_MESSAGE);

  strcpy(reverseMesg, recvMesg);
  reverseString(reverseMesg); //reverse the string for the final 
  if((!strncasecmp(recvMesg, "<echo>", ECHO_XML_START)) && (!strncasecmp(reverseMesg, ">ohce/<", ECHO_XML_END))) //compare beginning and ending ECHO
  {
	strcpy(sendMesg, "<reply>");
	strncpy(modifiedReceiveMessage, recvMesg + ECHO_XML_START, strlen(recvMesg) - (ECHO_XML_START + ECHO_XML_END));
	strcat(sendMesg, modifiedReceiveMessage);
	strcat(sendMesg, "</reply>");
  }
  else	
	errorMessage(recvMesg, sendMesg); 
}


/*
 **************************************************
 *	MODIFIED ON 2/6/2014
 *	Line 315: Changed to error check getloadavg
 **************************************************
 */
void loadavgMessage(char *recvMesg, char *send){
  char *sendMesg = send, time[MAX_MESSAGE];
  double loadAvg[LOAD_AVG_FUNCTION] = {0.0, 0.0, 0.0}; 
  if( getloadavg(loadAvg, LOAD_AVG_FUNCTION) < 0 ) {
  		strcpy(sendMesg, "<error>unable to obtain load average</error>");
		return;
  }
  strcpy(sendMesg, "<replyLoadAvg>");
  sprintf(time, "%f", loadAvg[LOAD_AVG_1_MIN_INDEX]);
  strcat(sendMesg, time);
  strcat(sendMesg, ":");
  sprintf(time, "%f", loadAvg[LOAD_AVG_5_MIN_INDEX]);
  strcat(sendMesg, time);
  strcat(sendMesg, ":");
  sprintf(time, "%f", loadAvg[LOAD_AVG_15_MIN_INDEX]);
  strcat(sendMesg, time);
  strcat(sendMesg, "</replyLoadAvg>");
}


/*
 **************************************************
 **************************************************
 */
void errorMessage(char *recvMesg, char *send){
  char *sendMesg = send;
  strcpy(sendMesg, "<error>unknown format</error>");
}


/*
 **************************************************
 **************************************************
 */
void reverseString(char *original){
  char temp;
  int i = 0, j = strlen(original) - NEW_LINE;
  for(i = 0; i < j; i++, j--)
  {	
	temp = original[i];
	original[i] = original[j];
	original[j] = temp;
  }
}



