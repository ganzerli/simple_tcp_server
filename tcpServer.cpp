#define WINVER 0x501
// defined windows version for the file of ws2tcpip.h

#include <iostream>
#include <WS2tcpip.h>
#include <winsock2.h>


// comment to use this library
#pragma comment (lib, "ws2_32.lib");
// to use this specific library must be specified it also in the compiler afre the files
// g++ -o srv.exe tcpServer.cpp -lws2_32

int main(){

	//initialize winsock,
	// call startup, requires data ws
	WSADATA wsData;
	//specify the version
	WORD version = MAKEWORD(2,2);
		
	int wsock = WSAStartup(version, &wsData);

// declaring functions for socket of windows api
	int WSAAPI getnameinfo(const struct sockaddr*,socklen_t,char*,DWORD,
		       char*,DWORD,int);


	PCSTR WSAAPI inet_ntop(
  			INT        Family,
  			const VOID *pAddr,
  			PSTR       pStringBuf,
  			size_t     StringBufSize
		);


	if(wsock != 0){
		std::cerr << "socket cannot be initialized" << std::endl;
		return 0;
	}
    // socket is only a noumber
	
	// create a socket
	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(listeningSocket == INVALID_SOCKET){
		std::cerr << "socket cannot be created" << std::endl;
	}
	
	// bind ip addr and port to socket
	sockaddr_in hint;

	// is needed the hint structure, networking is big andien. pc is little andien
	hint.sin_family = AF_INET;
	hint.sin_port = htons(5400);
	// bind to any address.. other could be the inet_pton.
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	//bind the port to the socket
	bind(listeningSocket , (sockaddr*)&hint , sizeof(hint));


	// listen for connection
	
	listen(listeningSocket , SOMAXCONN);
	// info to winsock about socket

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listeningSocket , (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST]; 				// Client's remote name
	char service[NI_MAXHOST];			// Service (i.e. port) the client is connected to

	ZeroMemory(host, NI_MAXHOST);		// same as memset(host,o,NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);	// memset linux or others os?

	// init getnameinfo 
	DWORD nameInfo = getnameinfo(
		(sockaddr*)&client, 
		sizeof(client), 
		host, 
		NI_MAXHOST, 
		service, 
		NI_MAXSERV,
		0	);

	// check if htere is the name information.. lookup the hostname and display that
	if(nameInfo == 0 ){
		// getnameinfo fills the memory for host and service
		std::cout << host << " connected on port " << service << "\n";
	}else{
		// returns the ipv4
		std::cout << host << " somehow unction inet_ntop is not working, because MinGW does not include this library "<< std::endl;
		/*
			#include <arpa/inet.h>
			const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
			int inet_pton(int af, const char *src, void *dst);
		*/
		//inet_ntop(AF_INET, &client.sin_addr,host, NI_MAXHOST);
														//network to host short ntohs
		//std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}


	// close socket
	closesocket(listeningSocket);

	// loop to accept message and echo back to the client
	char buffer[4096];

	while(true){
		// replace the memory for the buffer
		ZeroMemory(buffer, 4096);

		// waiting client to send data, receive function returns a number of byte recived, if is 0 client is disconnected
		int bytesReceived = recv(clientSocket, buffer, 4096,0);
		//echo back the message
		if(bytesReceived == 0){
			std::cerr << "error in recv(), client may be disconnected" <<std::endl;
			break;
		}
		if(bytesReceived == SOCKET_ERROR){
			std::cerr << "error in recv()" <<std::endl;
			break;
		}

		//echoing back to the client the message
		send(clientSocket, buffer, bytesReceived+1, 0);
	}

	//close socket
	closesocket(clientSocket);
	//shutdown winsock
	WSACleanup();

	std::cout << "supernice!! \n";
	
	return 1;
}