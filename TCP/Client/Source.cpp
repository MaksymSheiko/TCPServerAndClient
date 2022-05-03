#pragma once
#include "Client.h"

bool SetupWSA() 
{
	WSAData wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int main()
{
	if (!SetupWSA()) return 1;
	Client client;
	if (int res = client.StartupConnection("127.0.0.1", 13131) != 0)
	{
		std::cout << "Error code " << res << std::endl;
		return 1;
	}
	client.Listen();
	WSACleanup();
	return 0;
}