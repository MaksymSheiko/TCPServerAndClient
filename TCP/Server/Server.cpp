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
	m_clients.emplace("Server", m_socket);
	return 0;
}

void Server::Listen()
{
	const int BACKLOG = 1000;
	fd_set current_sockets, ready_sockets;
	FD_ZERO(&current_sockets);
	FD_SET(m_socket, &current_sockets);
	listen(m_socket, BACKLOG);
	while (true)
	{
		ready_sockets = current_sockets;
		select(FD_SETSIZE, &ready_sockets, nullptr, nullptr, nullptr);
		for (const auto& it : m_clients)
		{
			if (FD_ISSET(it.second, &ready_sockets))
			{
				if (it.second == m_socket)
				{
					SOCKET new_client = AcceptConnection();
					if (new_client != INVALID_SOCKET)
					{
						std::cout << "New connection on socket : " << new_client << std::endl;
						m_clients.emplace(std::to_string(new_client), new_client);
						FD_SET(new_client, &current_sockets);
					}
				}
				else
				{
					std::string rcv_str;
					PacketType packetType;
					if(ReadData(it.second, rcv_str, packetType))
					{
						if (packetType == PacketType::Echo)
						{
							std::cout << "Message from " << it.first << " : " << rcv_str << std::endl;
							SendData(it.second, rcv_str);
						}
						else if (packetType == PacketType::SetupLogin)
						{
							SOCKET client_socket = it.second;
							m_clients.erase(m_clients.find(it.first));
							if (m_clients.find(rcv_str) != m_clients.end())
							{
								int i = 1;
								for (; m_clients.find(rcv_str + std::to_string(i)) != m_clients.end(); ++i);
								rcv_str += std::to_string(i);
							}
							m_clients.emplace(rcv_str, client_socket);
						}
						else
						{
							std::cout << "Erorr! Unknown packet type!" << std::endl;
						}
					}
					else
					{
						SOCKET client_socket = it.second;
						m_clients.erase(m_clients.find(it.first));
						FD_CLR(client_socket, &current_sockets);
						closesocket(client_socket);
					}
				}
			}

		}
	}
}

SOCKET Server::AcceptConnection()
{
	sockaddr_storage socket_addr;
	socklen_t socket_len = sizeof(socket_addr);
	return accept(m_socket, (sockaddr*)&socket_addr, &socket_len);
}

bool Server::ReadData(SOCKET client_socket, std::string& message, PacketType& packetType)
{
	char buff[g_buffer_size];
	int bytesReceived = recv(client_socket, buff, g_buffer_size, 0);
	if (bytesReceived > 0)
	{
		Packet packet(buff);
		packetType = static_cast<PacketType>(packet.ReadInt());
		message = packet.ReadString();
		return true;
	}
	else
	{
		return false;
	}
}

void Server::SendData(SOCKET client_socket, std::string message)
{
	if (client_socket != INVALID_SOCKET)
	{
		Packet packet;
		packet.Write(message);
		packet.WriteLength();
		int bytesSend = send(client_socket, packet.GetData(), g_buffer_size, 0);
	}
}
