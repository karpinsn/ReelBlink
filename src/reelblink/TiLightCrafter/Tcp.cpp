#include "Tcp.h"


SOCKET Tcp::TCP_Connect(string host, string port)
{
   
  
  WSADATA wsaData;

  int iResult;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
	printf("WSAStartup failed: %d\n", iResult);
	return 1;
  }

  struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;

  ZeroMemory( &hints, sizeof(hints) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  iResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
  if (iResult != 0) {
	  printf("getaddrinfo failed: %d\n", iResult);
	  WSACleanup();
	  return 1;
  }

  SOCKET ConnectSocket = INVALID_SOCKET;

  // Attempt to connect to the first address returned by
  // the call to getaddrinfo
  ptr=result;

  // Create a SOCKET for connecting to server
  ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
	  ptr->ai_protocol);

  if (ConnectSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  // Connect to server.
  iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
	  closesocket(ConnectSocket);
	  ConnectSocket = INVALID_SOCKET;
  }

  // Should really try the next address returned by getaddrinfo
  // if the connect call failed
  // But for this simple example we just free the resources
  // returned by getaddrinfo and print an error message

  freeaddrinfo(result);

  if (ConnectSocket == INVALID_SOCKET) {
	  printf("Unable to connect to server!\n");
	  WSACleanup();
	  return 1;
  }
 

	return ConnectSocket ;
}

int Tcp::TCP_Send(int sock, unsigned char *buffer, int length)
{
    // Send 'length' number of bytes from buffer via provided
    // socket <sock> address
   
  
  return send(sock, (char *)buffer, length, 0);
}

int Tcp::TCP_Receive(int sock, unsigned char *buffer, int length)
{
    //Retrieve 'length' number of bytes into 'buffer' from the socket <sock> address
  return recv(sock, (char*) buffer, length, 0); 
}

bool Tcp::TCP_Disconnect(int sock)
{
    //free and cleanup socket occupied memory
	// if connected
	int socketCloced;
	int wsaCleaned;

	socketCloced = closesocket(sock);
	wsaCleaned = WSACleanup();
    
	if(socketCloced ==0 && wsaCleaned == 0)
	{
	  return true;
	}
	else
	{
	  return false;
	}
      
}
