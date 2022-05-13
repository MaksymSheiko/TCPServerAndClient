#pragma once
#include <iostream>
#include <string>
#include <thread>
#include "Client.h"

#pragma comment (lib, "ws2_32.lib")

class Server
{
public:
	Server() 
		: m_socket()
		, m_address() 
	{

	}
	~Server()
	{
		closesocket(m_socket);
	}
	int SetupServer(int port);
	void Listen();
private:
	SOCKET AcceptConnection();
	void StartClientThread(Client&& client);
private:
	SOCKET m_socket;
	sockaddr_in m_address;
};

