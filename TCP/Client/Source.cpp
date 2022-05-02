#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include "Packet.h"
#include <string>
#pragma comment (lib, "ws2_32.lib")

SOCKET SetupClient()
{
	const char* localhost = "127.0.0.1";
	int port = 13131;
	sockaddr_in server_address;
	socklen_t server_address_len = sizeof(server_address);
	SOCKET client_socket;

	WSAData wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0) return -1;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		return 3;
	}
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	if (inet_pton(AF_INET, localhost, &server_address.sin_addr) <= 0)
	{
		return 1;
	}
	if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0)
	{
		return 2;
	}
	return client_socket;
}

int main()
{
	SOCKET client_socket = SetupClient();
	std::string msg;
	while (true)
	{
		std::cout << "Enter message: " << std::endl;
		std::getline(std::cin, msg);
		Packet packet;
		packet.Write(msg);
		packet.WriteLength();
		if (send(client_socket, packet.GetData(), packet.GetSize() + 1, 0) > 0)
		{
			std::cout << "Message sent" << std::endl;
		}
		else
		{
			std::cout << "Error " << WSAGetLastError() << std::endl;
			break;
		}
		char buff[2048];
		int read = recv(client_socket, buff, 2048, 0);
		Packet receivePac(buff);
		std::cout <<"Echo from the server: " << receivePac.ReadString() << std::endl;
	}
	closesocket(client_socket);
	WSACleanup();
	return 0;
}