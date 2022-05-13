#include "Server.h"

int Server::SetupServer(int port)
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}
	m_address.sin_family = AF_INET;
	m_address.sin_port = htons(port);
	m_address.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(m_socket, (sockaddr*)&m_address, sizeof(m_address)) < 0)
	{
		return WSAGetLastError();
	}
	return 0;
}

void Server::Listen()
{
	const int BACKLOG = 1000;
	listen(m_socket, BACKLOG);
	while (true)
	{
		SOCKET client_socket = AcceptConnection();
		if (client_socket != INVALID_SOCKET)
		{
			std::thread t(&Server::StartClientThread, this, std::move(Client(client_socket)));
			t.detach();
		}
	}
}

SOCKET Server::AcceptConnection()
{
	sockaddr_storage socket_addr;
	socklen_t socket_len = sizeof(socket_addr);
	return accept(m_socket, (sockaddr*)&socket_addr, &socket_len);
}

void Server::StartClientThread(Client&& client)
{
	while (client.isValid())
	{
		std::string rcv_str;
		PacketType packetType;
		if (client.Read(rcv_str, packetType))
		{
			switch (packetType)
			{
			case PacketType::SetupLogin:
			{
				client.SetNickname(rcv_str);
				std::cout << "New client: " << client.GetNickname() << std::endl;
				break;
			}

			case PacketType::Echo:
			{
				std::cout << "Message from " << client.GetNickname() << " : " << rcv_str << std::endl;
				client.Send(rcv_str);
				break;
			}
			default:
			{
				std::cout << "Unknown packet type" << std::endl;
			}

			}
		}
		else
		{
			break;
		}
	}
}