#pragma once
#include <iostream>
#include <vector>

enum class PacketType
{
	SetupLogin = 0,
	Echo = 1,
};

class Packet
{
public:
	Packet();
	Packet(char* buff);
	~Packet();
	void Write(int data);
	void Write(const std::string& data);
	int ReadInt();
	std::string ReadString();

	void WriteLength();
	char* GetData();
	int GetSize();
private:
	std::vector<char> m_buff;
	int m_size;
};
