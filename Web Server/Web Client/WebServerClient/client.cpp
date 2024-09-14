#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using std::string,std::cout,std::endl,std::cin;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include <iomanip>

const int TIME_PORT = 27015;


void main()
{

	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	//
	// Binding the created socket to the IP address and port representing "us".
	//
	//Note: for clients, if the socket is left unbound, the first call for connect() will 
	//bind the socket with arbitrary properties (IP and Port)

	//If specific values are required, you can use the following code:

	//struct sockaddr_in me;
	//me.sin_family = AF_INET;
	//me.sin_addr.s_addr = INADDR_ANY;
	//me.sin_port = 0;
	//
	// Binding the created socket to the IP address and port representing "us".
	//
	//if (bind(connSocket, (struct sockaddr*)&me, sizeof(me)) == SOCKET_ERROR)
	//{
	//	cout<<"Time Server: Error at bind(): "<<WSAGetLastError()<<endl;
	//  closesocket(connSocket);
	//	WSACleanup();
	//  return;
	//}

	// For a client to communicate on a network, it must connect to a server.    
	// Need to assemble the required data for connection in sockaddr structure.
	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// Connect to server.

	// The connect function establishes a connection to a specified network 
	// address. The function uses the socket handler, the sockaddr structure 
	// (which defines properties of the desired connection) and the length of 
	// the sockaddr structure (in bytes).
	if (SOCKET_ERROR == connect(connSocket, (SOCKADDR*)&server, sizeof(server)))
	{
		cout << "Time Client: Error at connect(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return;
	}

	// Send and receive data.

	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[1040];
	char recvBuff[1040];
	int option = 0;

	while (option != 8)
	{
		cout << "\nPlease choose an option:\n";
		cout << "1 - Check GET.\n";
		cout << "2 - Check HEAD.\n";
		cout << "3 - check POST.\n";
		cout << "4 - check PUT.\n";
		cout << "5 - check DELETE.\n";
		cout << "6 - check TRACE.\n";
		cout << "7 - check OPTIONS.\n";
		cout << "8 - Exit.\n";


		cin >> option;

		if (option == 1)
			strcpy(sendBuff, "GET /index.html?lang=he HTTP/1.1\r\nHost: example.com\r\nAccept-Language: en, he, fr\r\n\r\n");
		else if (option == 2)
			strcpy(sendBuff, "HEAD /index.html?lang=he HTTP/1.1\r\nHost: example.com\r\nAccept-Language: en, he, fr\r\n\r\n");
		else if (option == 3)
			strcpy(sendBuff, "POST /helloWorld.txt HTTP/1.1\r\nHost: example.com\r\nContent-Type: text / plain\r\nContent-Length: 17\r\n\r\nhello world!!!!!!!");
		else if (option == 4)
			strcpy(sendBuff, "PUT /helloWorld.txt HTTP/1.1\r\nHost: example.com\r\nContent-Type: text / plain\r\nContent-Length: 17\r\n\r\nhello world!!!!!!!");
		else if (option == 5)
			strcpy(sendBuff, "DELETE /helloWorld.txt HTTP/1.1\r\nHost: example.com\r\nContent-Type: text / plain\r\nContent-Length: 17\r\n\r\nhello world!!!!!!!");
		else if (option == 6)
			strcpy(sendBuff, "TRACE /helloWorld.txt HTTP/1.1\r\nHost: example.com\r\nContent-Type: text / plain\r\nContent-Length: 17\r\n\r\nhello world!!!!!!!");
		else if (option == 7)
			strcpy(sendBuff, "OPTIONS /helloWorld.txt HTTP/1.1\r\nHost: example.com\r\nContent-type: text / plain\r\nContent-Length: 17\r\n\r\nhello world!!!!!!!");
		else if (option == 8)
			break;
		else
		{
			cout << "Invalid option\n";
			continue;
		}
		// The send function sends data on a connected socket.
		// The buffer to be sent and its size are needed.
		// The last argument is an idicator specifying the way 
		// in which the call is made (0 for default).
		bytesSent = send(connSocket, sendBuff, (int)strlen(sendBuff), 0);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Client: Error at send(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return;
		}
		cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";

		// Gets the server's answer for options 1 and 2.
		if (option != 8)
		{
			bytesRecv = recv(connSocket, recvBuff, 1040, 0);
			if (SOCKET_ERROR == bytesRecv)
			{
				cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
				closesocket(connSocket);
				WSACleanup();
				return;
			}
			if (bytesRecv == 0)
			{
				cout << "Server closed the connection\n";
				return;
			}

			recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
			cout << recvBuff << endl;
		}
		else if (option == 8)
		{
			// Closing connections and Winsock.
			cout << "Time Client: Closing Connection.\n";
			closesocket(connSocket);
			WSACleanup();
		}
	}
}