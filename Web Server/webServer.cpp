#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "inOutHelper.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#define CheckAllocation(c) if(c==NULL){ \
printf("Memory Allocation Failed!!!");   \
exit(1);                                \
}
using std::string, std::cout, std::endl;

struct massage_headers
{
	string method;
	string path;
	string protocol;
	string host;
	string accept_languages;
	//string file_name;
	string content_len;
	string body;
}typedef massage_headers;

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	int method;	// Sending sub-type
	massage_headers headers;
	char * buffer;

	//int len;
}typedef SocketState;

constexpr int TIME_PORT = 27015;
constexpr int MAX_SOCKETS = 60;
constexpr int EMPTY = 0;
constexpr int LISTEN = 1;
constexpr int RECEIVE = 2;
constexpr int IDLE = 3;
constexpr int SEND = 4;
constexpr int BUFFER_SIZE = 1024;

bool addSocket(SocketState sockets[], int& socketsCount, SOCKET id, int what);
void removeSocket(SocketState sockets[],int& socketsCount,int index);
void acceptConnection(SocketState sockets[],int& socketsCount,int index);
void receiveMessage(SocketState sockets[],int& socketsCount, int index);
void sendMessage(SocketState sockets[],int index);
void parseMassage(char massage[], massage_headers& headers);
void handleReq(massage_headers& headers, char* response);



void main()
{
	SocketState sockets[MAX_SOCKETS] = { 0 };
	int socketsCount = 0;
	
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == listenSocket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	
	sockaddr_in serverService;
	
	serverService.sin_family = AF_INET;

	serverService.sin_addr.s_addr = INADDR_ANY;

	serverService.sin_port = htons(TIME_PORT);

	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	if (SOCKET_ERROR == listen(listenSocket, 5))
	{
		cout << "Time Server: Error at listen(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}
	addSocket(sockets,socketsCount,listenSocket, LISTEN);

	while (true)
	{
		// The select function determines the status of one or more sockets,
		// waiting if necessary, to perform asynchronous I/O. Use fd_sets for
		// sets of handles for reading, writing and exceptions. select gets "timeout" for waiting
		// and still performing other operations (Use NULL for blocking). Finally,
		// select returns the number of descriptors which are ready for use (use FD_ISSET
		// macro to check which descriptor in each set is ready to be used).
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if ((sockets[i].recv == LISTEN) || (sockets[i].recv == RECEIVE))
				FD_SET(sockets[i].id, &waitRecv);
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (sockets[i].send == SEND)
				FD_SET(sockets[i].id, &waitSend);
		}

		//
		// Wait for interesting event.
		// Note: First argument is ignored. The fourth is for exceptions.
		// And as written above the last is a timeout, hence we are blocked if nothing happens.
		//
		int nfd;
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
		if (nfd == SOCKET_ERROR)
		{
			cout << "Time Server: Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitRecv))
			{
				nfd--;
				switch (sockets[i].recv)
				{
				case LISTEN:
					acceptConnection(sockets,socketsCount,i);
					break;

				case RECEIVE:
					receiveMessage(sockets,socketsCount,i);
					break;
				}
			}
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitSend))
			{
				nfd--;
				if (sockets[i].send == SEND)
				{
					sendMessage(sockets,i);
				}
			}
		}
	}
	/* we will never reach here !!!
	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();
	*/
}

bool addSocket(SocketState sockets[], int& socketsCount, SOCKET id, int state)
{
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].recv == EMPTY)
		{
			sockets[i].id = id;
			sockets[i].recv = state;
			sockets[i].send = IDLE;
			//sockets[i].len = 0;
			socketsCount++;
			//
			// Set the socket to be in non-blocking mode.
			//
			unsigned long flag = 1;
			if (ioctlsocket(id, FIONBIO, &flag) != 0)
			{
				cout << "Time Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
			}
			return (true);
		}
	}
	return (false);
}

void removeSocket(SocketState sockets[],int& socketsCount,int index)
{
	sockets[index].recv = EMPTY;
	sockets[index].send = EMPTY;
	socketsCount--;
}

void acceptConnection(SocketState sockets[],int& socketsCount,int index)
{
	SOCKET id = sockets[index].id;
	struct sockaddr_in from;		// Address of sending partner
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket)
	{
		cout << "Time Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "Time Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;

	if (addSocket(sockets,socketsCount,msgSocket, RECEIVE) == false)
	{
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
	return;
}

void receiveMessage(SocketState sockets[],int& socketsCount,int index)
{
	SOCKET msgSocket = sockets[index].id;
	int phyicalLength = BUFFER_SIZE;
	int logicalLength = 0;
	sockets[index].buffer = (char*)malloc(phyicalLength * sizeof(char));
	CheckAllocation(sockets[index].buffer);
	int bytesRecv = recv(msgSocket, sockets[index].buffer, sizeof(sockets[index].buffer), 0);
	while(bytesRecv != -1)
	{
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(msgSocket);
			removeSocket(sockets, socketsCount, index);
			return;
		}
		if (bytesRecv == 0)
		{
			closesocket(msgSocket);
			removeSocket(sockets, socketsCount, index);
			return;
		}
		logicalLength += bytesRecv;
		if (logicalLength == phyicalLength)
		{
			phyicalLength *= 2;
			sockets[index].buffer = (char*)realloc(sockets[index].buffer, phyicalLength * sizeof(char));
			CheckAllocation(sockets[index].buffer);
		}
		bytesRecv = recv(msgSocket, &sockets[index].buffer[logicalLength], sizeof(sockets[index].buffer) - logicalLength, 0);
	}
		sockets[index].buffer[logicalLength] = '\0'; //add the null-terminating to make it a string
		cout << "Web Server: Recieved: " << bytesRecv << " bytes of \"" << sockets[index].buffer << "\" message.\n";
		parseMassage(sockets[index].buffer, sockets[index].headers);
}

void parseMassage(char massage[], massage_headers& headers)
{
	char* token = strtok(massage, "\r\n");
	if (token != NULL) 
	{
		headers.method = strtok(token, " ");
		headers.path = strtok(NULL, " ");
		headers.protocol = strtok(NULL, " ");
	}
	token = strtok(NULL, "\r\n");
	while (token != NULL)
	{
		if (strstr(token, "Host:") != NULL)
		{
			strtok(token, " ");
			headers.host = strtok(NULL, " ");
		}
		else if (strstr(token, "Accept-Language:") != NULL)
		{
			strtok(token, " ");
			headers.accept_languages = strtok(NULL, " ");
		}
		else if (strstr(token, "Content-Length:") != NULL)
		{
			strtok(token, " ");
			headers.content_len = strtok(NULL, " ");
		}
		token = strtok(NULL, "\r\n");
	}
	if(!headers.content_len.empty() && headers.content_len != "0")
	{
		token = strstr(massage, "\r\n\r\n") + 4;
		headers.body = token;
	}
}



void sendMessage(SocketState sockets[], int index)
{
	int bytesSent = 0;
	char * sendBuff = nullptr;

	SOCKET msgSocket = sockets[index].id;
	handleReq(sockets[index].headers, sendBuff);
	bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}

	cout << sendBuff << endl;

	sockets[index].send = IDLE;
	free(sockets[index].buffer);
}
void handleReq(massage_headers& headers,char* response)
{
	if (headers.method == "GET")
	{
		GET_HEAD_request(headers.path, response, GET, headers.accept_languages);
	}
	else if (headers.method == "HEAD")
	{
		GET_HEAD_request(headers.path, response, HEAD, headers.accept_languages);
	}
	else if (headers.method == "POST")
	{
		POST_request(headers.body, response); 
	}
	else if (headers.method == "PUT")
	{
		PUT_request(headers.path, headers.body, response);
	}
	else if (headers.method == "DELETE")
	{
		DELETE_request(headers.path, response);
	}
	else if (headers.method == "OPTIONS")
	{
		createResponse(GOOD, HTTP_TYPE, response, OPTIONS);
	}
	else
	{
		createResponse(NOT_ALLWOED, HTTP_TYPE, response, -1);
	}
}
	
