import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.StringTokenizer;

/*
 * UDPclient.java
 * Systems and Networks II
 * Project 2
 *
 * This file describes the functions to be implemented by the UDPclient class
 * You may also implement any auxiliary functions you deem necessary.
 */

public class UDPclient {
	// variables for network connection
	private DatagramSocket _socket; // the socket for communication with a server
	private InetAddress servAddr;
	private int servPort;
	// Constants
	private static final int BUFFSIZE =256; //
	private static final int ERROR = -1;
	private static final int SUCCESS = 0;
	private static final int MILISEC_TIMEOUT = 500;//half second
	//Message handling
	private String request;
	private String response;

	/**
	 * Constructs a TCPclient object.
	 */
	public UDPclient() {
		//TODO intentionally left blank
	}

	/**
	 * Creates a datagram socket and binds it to a free port
	 * 
	 * @return - 0 or a negative number describing an error code if the
	 *         connection could not be established
	 */
	public int createSocket() {
		try {
			_socket = new DatagramSocket();
			_socket.setSoTimeout(MILISEC_TIMEOUT);
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			//TODO What error code would this be?
		        System.err.println("Unable to create socket");
			return ERROR;
		}
		return SUCCESS;
	}

	/**
	 * Sends a request for service to the server. Do not wait for a reply in
	 * this function. This will be an asynchronous call to the server.
	 * 
	 * @param request
	 *            - the request to be sent
	 * @param hostAddr
	 *            - the ip or hostname of the server
	 * @param port
	 *            - the port number of the server
	 * 
	 * @return - 0, if no error; otherwise, a negative number indicating the
	 *         error
	 */
	public int sendRequest(String request, String hostAddr, int port) {
		this.request = request;
		this.request += '\u0000';
		//Check request string for errors in the client?????
		servPort = port;
		if(!doServAddr(hostAddr)){
		    
		    return ERROR;	//unknown host exception 
		}
		if(!doSendRequest()){
			return ERROR;
		}
		return SUCCESS;
	}
	/**
	 *    An instance of an InetAddress consists of an IP address and possibly its corresponding
	 *     host name (depending on whether it is constructed with a host name or whether it has already
	 *      done reverse host name resolution). 
	 *      This function initializes the the IP address of the server (i.e servAddr instance variable).
	 *      @return true if no error, false if a UknownHostException occurs
	 */
	private boolean doServAddr(String hostAddr){
		try {
			//TODO i think the name hostAddr is misleading, it should be called something like servName 
			servAddr = InetAddress.getByName(hostAddr);
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
		        System.out.println("Unknown host");
			closeSocket();
			return false;
		}
		return true;
	}
	
	/**
	 * this method does the actual sending of the request message
	 * @return true is no error, otherwise returns false
	 */
	private boolean doSendRequest(){
		try{
			//TODO
			byte[] sendBuff = new byte[BUFFSIZE];
			sendBuff = request.getBytes();
			DatagramPacket sendPacket = new DatagramPacket(sendBuff, sendBuff.length, servAddr, servPort);
			_socket.send(sendPacket);
		}catch(Exception ex){
			//TODO the book has a different error message than the one i put here
			//I am not quite sure what they meant by it
			//BOOK -> System.err.println("SocketException in getRequest");
			System.err.println("Send Request failed in doSendRequest()");
			closeSocket();
			return false;
		}
		return true;
	}
	
	/**
	 * Receives the server's response following a previously sent request.
	 * 
	 * @return - the server's response or NULL if an error occured
	 */
	public String receiveResponse() {
	    byte[] recvBuff = new byte[BUFFSIZE];
		try{
			
			DatagramPacket recvPacket = new DatagramPacket(recvBuff, BUFFSIZE);
			try{
			_socket.receive(recvPacket);
			}catch(SocketTimeoutException ex){
			        closeSocket();
				return response = "Server not responding";
			}
		}catch (Exception ex){
			//TODO the book has a different error message than the one i put here
			//I am not quite sure what they meant by it
			//BOOK -> System.err.println("SocketException in getRequest");
			System.err.println("SocketException in receiveResponse()");
			closeSocket();
			return null;
		}
		response = new String(recvBuff);
		return response;
	}

	/*
	 * Prints the response to the screen in a formatted way.
	 * 
	 * response - the server's response as an XML formatted string
	 */
	public static void printResponse(String response) {
		if(response.contains("<reply>")) {
			System.out.println("Reply: " + response);
		} else if(response.contains("<replyLoadAvg>")) {
			System.out.println("LoadAvg: " + response);
		} else if(response.contains("<replyShutDown>")) {
			System.out.println("ShutDown: " + response);
		} else {
			System.out.println("Unknown format Response: " + response);
		}
	}

	/*
	 * Closes an open socket.
	 * 
	 * @return - 0, if no error; otherwise, a negative number indicating the
	 * error
	 */
	public int closeSocket() {
		try{
		_socket.close();
		}catch(Exception ex){
			System.err.println("Exception in closeSocket()");
			return ERROR;
		}
		return SUCCESS;
	}

	/**
	 * The main function. Use this function for testing your code. We will
	 * provide a new main function on the day of the lab demo.
	 */
	public static void main(String[] args) {

	}

}
