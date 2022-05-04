#include "Client.h"

int Client::StartupConnection(const char* address, int port)
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	m_address.sin_family = AF_INET;
	m_address.sin_port = htons(port);
	if (inet_pton(AF_INET, address, &m_address.sin_addr) <= 0)
	{
		return WSAGetLastError();
	}
	if (connect(m_socket, (sockaddr*)&m_address, sizeof(m_address)) < 0)
	{
		return WSAGetLastError();
	}
	return 0;
}

void Client::Listen()
{
	std::string message;
	{
		std::cout << "Connection established. Enter your nickname:" << std::endl;
		std::getline(std::cin, message);
		Packet packet;
		packet.Write(static_cast<int>(PacketType::SetupLogin));
		packet.Write(message);
		packet.WriteLength();
		if (!Send(std::move(packet)))
		{
			return;
		}
	}
	while (true)
	{
		std::getline(std::cin, message);
		Packet packet;
		packet.Write(static_cast<int>(PacketType::Echo));
		packet.Write(message);
		packet.WriteLength();
		if (!Send(std::move(packet)))
		{
			break;
		}
		std::cout << "Echo: " << Receive() << std::endl;
	}
}

bool Client::Send(Packet&& packet)
{
	if (send(m_socket, packet.GetData(), packet.GetSize() + 1, 0) < 0)
	{
		closesocket(m_socket);
		return false;
	}
	return true;
}

std::string Client::Receive()
{
	char buff[2048];
	int read = recv(m_socket, buff, 2048, 0);
	Packet receivePacket(buff);
	return receivePacket.ReadString();
}
