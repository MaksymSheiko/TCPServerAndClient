#pragma once
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "Packet.h"
#include <map>

#pragma comment (lib, "ws2_32.lib")

const int g_buffer_size = 2048;

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
	bool ReadData(SOCKET client_socket, std::string& message, PacketType& packetType);
	void SendData(SOCKET client_socket, std::string message);
private:
	SOCKET m_socket;
	sockaddr_in m_address;
	std::map<std::string, SOCKET> m_clients;
};

