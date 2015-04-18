/**	@file	UDPserverMain.c
 * 	@brief	Contains the main program for starting the UDP server. 
 *	Calls all functions to create server socket and connections and
 *	begin running the server. 
 * 	@author Cole Amick
 * 	@author Daniel Davis
 * 	@bug No known bugs!
 */
 
#include "UDPserver.h"

/**	@brief 	The main program for running the TCP server.
*	@param 	argc is the number of command line arguments 
*			argv is the matrix array containing the command line arguments 
*	@return returns 0 to the OS when main completes. 
*/
int main(int argc, char **argv){

  int sockfd;
  struct hostent *hostptr; 
  struct sockaddr_in servaddr;

  if(argc == 2){
    sockfd = create_UDP_Socket();  //create the UDP socket
    hostptr = info_Host(); //get the server host
    servaddr = destination_Address(hostptr, atoi(argv[1])); //get the server IP address and argv[1] = server port number 
    servaddr = bind_Socket(sockfd, servaddr); //bind a socket for the server program 
    print_Server_info(sockfd, hostptr, servaddr); //print the server info
	run_Server(sockfd); //run the server program and wait for incoming client connections
  }
  else {
  	printf("Incorrect Number of Command Line Arguments\n");
	printf("./server <Port Number>\n");
  }
  return 0;
}

