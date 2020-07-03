#include "pch.h"
#include "SLCPClient.h"

int _tmain(int argc, TCHAR *argv[])
{
	SLCPClient slcpClient;
	std::cout << "> SLCP Client Start ..." << std::endl;

	if (!slcpClient.Run())
		std::cout << "> SLCP Client End with Unexpected ERROR..." << std::endl;
	else
		std::cout << "> SLCP Client End ..." << std::endl;

	return 0;
}

