#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include "Packet.h"

#pragma comment (lib, "ws2_32.lib")

const int g_port = 13131;
const int COUNT_OF_SOCKETS = 1000;
const int BUFFER_SIZE = 2048;

SOCKET SetupServer()
{
	WSAData wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cout << "Error while creating WSA " << WSAGetLastError() << std::endl;
	}

	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET)
	{
		std::cout << "Error while creating socket " << WSAGetLastError() << std::endl;
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(g_port);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cout << "Error while binding " << WSAGetLastError() << std::endl;
	}
	return server_socket;
}
SOCKET AcceptClientConnection(SOCKET server_socket)
{
	sockaddr_storage socket_addr;
	socklen_t socket_len = sizeof(socket_addr);
	return accept(server_socket, (sockaddr*)&socket_addr, &socket_len);
}
std::string ReadData(SOCKET socket)
{
	char buff[BUFFER_SIZE];
	int bytesReceived = recv(socket, buff, BUFFER_SIZE, 0);
	if (bytesReceived > 0)
	{
		Packet packet(buff);
		return packet.ReadString();
	}
	closesocket(socket);
	return "";
}
void SendData(SOCKET socket, std::string msg)
{
	if (socket != INVALID_SOCKET)
	{
		Packet packet;
		packet.Write(msg);
		packet.WriteLength();
		int bytesSend = send(socket, packet.GetData(), BUFFER_SIZE, 0);
	}
}
int main()
{
	SOCKET server_socket = SetupServer();
	fd_set current_sockets, ready_sockets;
	FD_ZERO(&current_sockets);
	FD_SET(server_socket, &current_sockets);
	listen(server_socket, COUNT_OF_SOCKETS);
	while (true)
	{
		ready_sockets = current_sockets;
		select(COUNT_OF_SOCKETS, &ready_sockets, nullptr, nullptr, nullptr);
		for (int i = 0; i < COUNT_OF_SOCKETS; ++i)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (i == server_socket)
				{
					SOCKET client_socket = AcceptClientConnection(server_socket);
					if (client_socket != INVALID_SOCKET)
					{
						std::cout << "Connected user, id: " << client_socket << std::endl;
						FD_SET(client_socket, &current_sockets);
					}
				}
				else
				{
					std::string rcv_str = ReadData(i);
					if (rcv_str.empty())
					{
						FD_CLR(i, &current_sockets);
					}
					else
					{
						std::cout << "Message from " << i << " : " << rcv_str << std::endl;
						SendData(i, rcv_str);
					}
				}
			}
		}
	}
	

	return 0;
}