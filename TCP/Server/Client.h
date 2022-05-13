#pragma once
#include <WS2tcpip.h>
#include "Packet.h"

#pragma comment (lib, "ws2_32.lib")

const int g_buffer_size = 2048;

class Client
{
public:
	Client(SOCKET client_socket) :
		m_socket(client_socket)
	{

	}
	Client(Client&& other) noexcept
	{
		this->m_socket = other.m_socket;
		other.m_socket = INVALID_SOCKET;
		this->m_nickname = std::move(other.m_nickname);
	}
	~Client()
	{
		if (isValid())
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
	}

	Client& operator= (Client&& other)
	{
		this->m_socket = other.m_socket;
		other.m_socket = INVALID_SOCKET;
		this->m_nickname = std::move(other.m_nickname);
		return *this;
	}

	bool isValid()
	{
		return m_socket != INVALID_SOCKET;
	}

	bool Read(std::string& message, PacketType& packetType)
	{
		char buff[g_buffer_size];
		int bytesReceived = recv(m_socket, buff, g_buffer_size, 0);
		if (bytesReceived > 0)
		{
			Packet packet(buff);
			packetType = static_cast<PacketType>(packet.ReadInt());
			message = packet.ReadString();
			return true;
		}
		else
		{
			std::cout << "Error " << WSAGetLastError() << std::endl;
			return false;
		}
	}

	void Send(std::string& message)
	{
		if (isValid())
		{
			Packet packet;
			packet.Write(message);
			packet.WriteLength();
			int bytesSend = send(m_socket, packet.GetData(), g_buffer_size, 0);
		}
	}

	void SetNickname(std::string& nickname)
	{
		m_nickname = nickname;
	}
	
	std::string& GetNickname()
	{
		return m_nickname;
	}

private:
	SOCKET m_socket;
	std::string m_nickname;
};

