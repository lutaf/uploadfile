#include <string.h>
#include "SimpleSocket.h"
#define RECV_BUFFER_SIZE (32*1024)
bool CSimpleSocket::Connect(const char* host, unsigned short port)
{
	struct sockaddr_in addr;
	memset((void *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host);
	if(addr.sin_addr.s_addr ==  INADDR_NONE)
	{
		hostent * hp=gethostbyname(host);
		if(hp)
		{
			memcpy(&(addr.sin_addr),hp->h_addr,hp->h_length);

		}
		else
		{
			return false;
		}
	}
	addr.sin_port = htons(port);

	if (::connect(m_Socket, (struct sockaddr *)&addr, sizeof(addr))==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//-------------------------------------------------------------
bool CSimpleSocket::Init()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	int err = WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
	{
		throw sockerr (WSAGetLastError());
		return false;
	}
	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
	if( m_Socket == INVALID_SOCKET ) {
		return false;
	}
	return true;
}
//-------------------------------------------------------------
void CSimpleSocket::Close()
{
	if( m_Socket != INVALID_SOCKET )
	{
		shutdown(m_Socket,SD_RECEIVE);
		closesocket( m_Socket );
		m_Socket= INVALID_SOCKET;
	}
}
//-------------------------------------------------------------
void CSimpleSocket::Send(const char* data,int len)
{
	int sendDone = 0;
	while(sendDone < len)
	{
		int sent =send(m_Socket,&data[sendDone],len-sendDone,0);
		if ( sent == SOCKET_ERROR )
		{
			throw sockerr (WSAGetLastError());
		}
		sendDone+=sent;
	}
}
//-------------------------------------------------------------
void CSimpleSocket::Read(std::string &buf)
{
	static char buffer[RECV_BUFFER_SIZE];
	buf.clear();
	while(1)
	{
		int bytesRecv = recv( m_Socket, buffer, RECV_BUFFER_SIZE, 0 );
		if(bytesRecv == 0)
		{
			Close();
			break;
		}
		else if(bytesRecv ==SOCKET_ERROR)
		{
			throw sockerr (WSAGetLastError());
		}
		buf.append(buffer,bytesRecv);
		if(bytesRecv < RECV_BUFFER_SIZE) break;
	}
}
//-------------------------------------------------------------
const char* sockerr::errstr () const
{
	static char buff[1024];
	sprintf(buff, "No.%d, ",m_err);
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		m_err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	strcat(buff, (char*)lpMsgBuf);
	return buff;
}