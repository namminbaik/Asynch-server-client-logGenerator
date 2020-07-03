#ifndef SLCP_CLIENT_H
#define SLCP_CLIENT_H

#include "SLCPMonitor.h"
#include "SLCPCommon.h"

class SLCPClient
{
public:
	SLCPClient();
	~SLCPClient();

	BOOL Run();
	BOOL Connect(PCWSTR szIPAdress, USHORT port);
	VOID CloseConnect();
	BOOL RunApplication(BOOL bIsCheckAlready = false);
	int FindApplication();
	VOID Receive();
	VOID ReceivedMsgHandling(const TCHAR* lpReceivedMsg);
	VOID SendMsg(int code);
	VOID SendMsg(int code, const TCHAR* lpFileName);
	BOOL LogFileDeletion(const TCHAR* lpFile);
	BOOL LogFileBackup(const TCHAR* lpFile);
	VOID ExitProgram();

	friend unsigned int __stdcall StatusCheck(void* obj);
	friend unsigned int __stdcall LogGenMonitor(void* obj);

private:
	SOCKET m_ClientSocket;
	SLCPMonitor m_Monitor;
	SHARED_MEMORY_DATA m_SharedData;
	TCHAR m_LogPath[MAX_PATH];
};

#endif // !SLCP_CLIENT_H	