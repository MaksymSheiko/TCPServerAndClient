#pragma once

#include "Server.h"

const int g_port = 13131;

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
	if (!SetupWSA())
	{
		return 1;
	}

	Server server;
	int res = server.SetupServer(g_port);
	if (res != 0)
	{
		std::cout << "Error: " << res << std::endl;
		return 1;
	}

	server.Listen();
	return 0;
}