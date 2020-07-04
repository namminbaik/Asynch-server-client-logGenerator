#include "pch.h"
#include "SLCPClient.h"

///////////////////////////////////////////////////////////////
// This is for console windows to terminate with 'X' button. //
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
///////////////////////////////////////////////////////////////

SLCPClient slcpClient;

int _tmain(int argc, TCHAR *argv[])
{
	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		std::cout << "> Fail to set control handler ...\n";
		return 1;
	}

	//SLCPClient slcpClient;
	std::cout << "> SLCP Client Start ...\n";

	if (!slcpClient.Run())
		std::cout << "> SLCP Client End with Unexpected ERROR ...\n";
	else
		std::cout << "> SLCP Client End normally ...\n";

	return 0;
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	default:
		slcpClient.CloseConnect();
		//slcpClient.~SLCPClient();
		while (true);
		return FALSE;
	}
}

