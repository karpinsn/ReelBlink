#include "Tcp.h"

Tcp::Tcp( void ) : m_socket( INVALID_SOCKET )
{ }

bool Tcp::Connect(string host, string port)
{
  // If we are connected already, just return true
  if( Connected( ) )
	{ return true; }
  
  WSADATA wsaData;

  // Initialize Winsock
  if ( 0 != WSAStartup( MAKEWORD(2,2), &wsaData ) ) {
	cout << "WinSock startup failed" << endl;
	return false;
  }

  struct addrinfo *result = NULL,
                  *ptr = NULL,
                  hints;

  ZeroMemory( &hints, sizeof(hints) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if ( 0 != getaddrinfo(host.c_str(), port.c_str(), &hints, &result)) 
  {
	cout << "WinSock getaddrinfo failed" << endl;
	WSACleanup();
    return false;
  }

  // Attempt to connect to the first address returned by
  // the call to getaddrinfo
  ptr = result;

  // Create a SOCKET for connecting to server
  m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
  if ( m_socket == INVALID_SOCKET ) 
  {
	cout << "Error in socket() : " << WSAGetLastError( ) << endl;
    freeaddrinfo(result);
    WSACleanup();
    return false;
  }

  // Connect to server.
  if ( SOCKET_ERROR == connect( m_socket, ptr->ai_addr, (int)ptr->ai_addrlen ) ) 
  {
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
	freeaddrinfo(result);

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message
	cout << "Unable to connect to server!" << endl;
	WSACleanup();
	return false;
  }

  freeaddrinfo(result);
  return true;
}

bool Tcp::Disconnect( void )
{
  // If we are not connected, dont worry about it
  if( !Connected( ) )
	{ return true; }

  // free and cleanup socket occupied memory
  // if connected
  if(0 == closesocket( m_socket ) && 0 == WSACleanup( ) )
	{ return true; }
  
  return false;    
}

int Tcp::Send( unsigned char *buffer, int length)
{
  // Cant send if we are not connected
  if( !Connected( ) )
  { return SOCKET_ERROR; }

  // Send 'length' number of bytes from buffer via provided
  // socket <sock> address
  return send( m_socket, ( char* )buffer, length, 0 );
}

int Tcp::Receive( unsigned char *buffer, int length )
{
  // Cant receive if we are not connected
  if( !Connected( ) )
	{ return SOCKET_ERROR; }

  //Retrieve 'length' number of bytes into 'buffer' from the socket <sock> address
  return recv( m_socket, ( char* ) buffer, length, 0 ); 
}

bool Tcp::Connected( void )
  { return INVALID_SOCKET != m_socket; }