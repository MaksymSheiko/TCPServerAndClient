#pragma once
#include <iostream>
#include <vector>

class Packet
{
public:
	Packet();
	Packet(char* buff);
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
