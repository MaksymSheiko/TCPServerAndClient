#include "pch.h"
#include "Packet.h"

Packet::Packet()
	: m_size(0)
{
	m_buff.resize(2048, '0');
}

Packet::Packet(char* buff)
{
	m_buff.resize(2048, '!');
	memcpy(&m_size, buff, 4);
	memcpy(m_buff.data(), buff, m_size+4);
	int readsize = ReadInt();
	if (m_size + 4 != readsize)
	{
		std::cout << "Error!!! Got " << m_size<< " expected " << readsize << std::endl;
	}
}

Packet::~Packet()
{
	m_buff.clear();
}

int Packet::ReadInt()
{
	int toReturn;
	std::memcpy(&toReturn, m_buff.data(), 4);
	for (int i = 0; i < sizeof(4); ++i)
	{
		m_buff.erase(m_buff.begin());
	}
	m_size -= 4;
	return toReturn;
}

std::string Packet::ReadString()
{
	int length = ReadInt();
	std::string toReturn;
	for (int i = 0; i < length; ++i)
	{
		toReturn += m_buff[0];
		m_buff.erase(m_buff.begin());
	}
	return toReturn;
}

void Packet::WriteLength()
{
	char* bytes = reinterpret_cast<char*>(&m_size);
	memcpy(m_buff.data(), bytes, sizeof(int));
}

char* Packet::GetData()
{
	return &m_buff[0];
}

int Packet::GetSize()
{
	return m_size+4;
}

void Packet::Write(int data)
{
	char* bytes = reinterpret_cast<char*>(&data);
	memcpy(m_buff.data() + 4 + m_size, bytes, sizeof(int));
	m_size += sizeof(int);
}

void Packet::Write(const std::string& data)
{
	int length = static_cast<int>(data.length());
	Write(length);
	memcpy(m_buff.data() + 4 + m_size, data.c_str(), length);
	m_size += length;
}
