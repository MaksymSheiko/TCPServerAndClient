#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include "Packet.h"
#include <string>
#pragma comment (lib, "ws2_32.lib")


class Client
{
public:
	Client()
		: m_socket() 
		, m_address() 
	{

	}
	~Client()
	{
		closesocket(m_socket);
	}
	int StartupConnection(const char* address, int port);
	void Listen();
private:
	bool Send(Packet&& packet);
	std::string Receive();
private:
	SOCKET m_socket;
	sockaddr_in m_address;
};

