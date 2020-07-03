#include "pch.h"
#include "SLCPClient.h"

#include <Ws2tcpip.h>
#include <process.h>
#include <TlHelp32.h>
#include <strsafe.h>

#pragma comment(lib, "ws2_32.lib")

SLCPClient::SLCPClient()
	: m_ClientSocket{ INVALID_SOCKET }
{
	// Shared memory setting
	ZeroMemory(&m_SharedData, sizeof(m_SharedData));
	memcpy(&m_SharedData.logCreationPolicy, _T("0"), sizeof(_T("0")));
	memcpy(&m_SharedData.backupFileStorage, _T("0"), sizeof(_T("0")));

	// Log file path setting
	ZeroMemory(m_LogPath, sizeof(m_LogPath));
	_tgetcwd(m_LogPath, MAX_PATH);
	TCHAR* lpszFind = _tcsrchr(m_LogPath, _T('\\'));
	int nFindIndex = static_cast<int>(lpszFind - m_LogPath + 1);
	m_LogPath[nFindIndex] = _T('\0');
}

SLCPClient::~SLCPClient()
{
}

unsigned int __stdcall StatusCheck(void* obj)
{
	while (true)
	{
		Sleep(1000 * 60);
		((SLCPClient*)obj)->SendMsg(CHECK_STATUS);

		if (((SLCPClient*)obj)->FindApplication() == 0)
			((SLCPClient*)obj)->RunApplication(true);
	}

	return 0;
}

unsigned int __stdcall LogGenMonitor(void* obj)
{
	TCHAR strLogFileFind[MAX_PATH];
	ZeroMemory(strLogFileFind, sizeof(strLogFileFind));
	StringCbPrintf(strLogFileFind, sizeof(strLogFileFind), _T("%slog\\*"), ((SLCPClient*)obj)->m_LogPath);

	do
	{
		Sleep(1000);

		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(strLogFileFind, &fd);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
					((SLCPClient*)obj)->SendMsg(LOG_DATA_SEND, fd.cFileName);

			} while (FindNextFile(hFind, &fd));
		}
		FindClose(hFind);
	} while (true);

	return 0;
}

BOOL SLCPClient::Run()
{
	// Connect to server ///////////////////////////////////////////////
	if (Connect(NETWORK_ADDRESS, NETWORK_PORT) == false)
		return false;
	////////////////////////////////////////////////////////////////////


	// Create and set shared memory ////////////////////////////////////
	if (m_Monitor.CreateSharedMem() == false)
	{
		std::cout << "> Failed to Create shared memory ...\n";
		CloseConnect();
		return false;
	}

	m_Monitor.WriteSharedMem(m_SharedData);
	////////////////////////////////////////////////////////////////////


	// Check and process running ///////////////////////////////////////
	if (RunApplication() == false)
		std::cout << "> Failed to run application ...\n";
	////////////////////////////////////////////////////////////////////


	// Start status check thread ///////////////////////////////////////
	_beginthreadex(NULL, 0, StatusCheck, this, 0, NULL);
	////////////////////////////////////////////////////////////////////


	// Start log files monitor thread //////////////////////////////////
	_beginthreadex(NULL, 0, LogGenMonitor, this, 0, NULL);
	////////////////////////////////////////////////////////////////////


	// listen from server //////////////////////////////////////////////
	Receive();
	////////////////////////////////////////////////////////////////////


	// Exit Program ////////////////////////////////////////////////////
	ExitProgram();
	////////////////////////////////////////////////////////////////////
	return true;
}

BOOL SLCPClient::Connect(PCWSTR szIPAdress, USHORT port)
{
	// Create Socket ///////////////////////////////////////////////////
	if (m_ClientSocket != INVALID_SOCKET)
	{
		std::cout << "> Socket is aleady open ...\n";
		return false;
	}

	WSADATA stWsaData;
	int nResult = WSAStartup(MAKEWORD(2, 2), &stWsaData);
	if (nResult != 0)
	{
		std::cout << "> Failed to initialize Winsock ...\n";
		return false;
	}

	m_ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ClientSocket == INVALID_SOCKET)
	{
		std::cout << "> Failed to create Socket ...\n";
		WSACleanup();
		return false;
	}
	////////////////////////////////////////////////////////////////////


	// Connect to server ///////////////////////////////////////////////
	SOCKADDR_IN stSin;

	stSin.sin_family = AF_INET;
	InetPton(AF_INET, szIPAdress, &stSin.sin_addr.s_addr);
	stSin.sin_port = htons(port);

	nResult = connect(m_ClientSocket, (sockaddr *)&stSin, sizeof(stSin));

	if (nResult == SOCKET_ERROR)
	{
		std::cout << "> Failed to connect to Server ...\n";
		CloseConnect();
		return false;
	}
	////////////////////////////////////////////////////////////////////

	return true;
}

VOID SLCPClient::CloseConnect()
{
	if (m_ClientSocket != INVALID_SOCKET)
		closesocket(m_ClientSocket);

	WSACleanup();
}

BOOL SLCPClient::RunApplication(BOOL bIsCheckAlready)
{
	int nResult = 0;
	if (!bIsCheckAlready)
		nResult = FindApplication();

	switch (nResult)
	{
	case -1:
		return false;
	case 0:
		TCHAR strCurPath[MAX_PATH];
		TCHAR strAppName[MAX_PATH];
		ZeroMemory(strCurPath, sizeof(strCurPath));
		ZeroMemory(strAppName, sizeof(strAppName));

		_tgetcwd(strCurPath, MAX_PATH);
		StringCbPrintf(strAppName, sizeof(strAppName), _T("%s\\%s"), strCurPath, PROCESS_NAME);

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		bool isCreateSuccess = CreateProcess(strAppName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		if (!isCreateSuccess)
			return false;
	}

	int cnt = 0;
	while (cnt < 10)
	{
		if (m_Monitor.ConnectEvent() == true)
			break;
		cnt++;
		Sleep(100);
	}

	if (cnt == 10)
		std::cout << "> Failed to connect to event ...\n";
	else
		m_Monitor.SendEvent();

	return true;
}

int SLCPClient::FindApplication()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return -1;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	int nFind = 0;

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return -1;
	}

	do
	{
		if (!_tcscmp(pe32.szExeFile, PROCESS_NAME))
		{
			nFind = 1;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return nFind;
}

VOID SLCPClient::Receive()
{
	SendMsg(POLICY_REQUEST);
	TCHAR szReceiveMsg[BUFFER_SIZE];

	while (true)
	{
		ZeroMemory(szReceiveMsg, sizeof(szReceiveMsg));
		int nRecvLen = recv(m_ClientSocket, (char*)szReceiveMsg, sizeof(szReceiveMsg), 0);

		if (nRecvLen == 0)
		{
			std::cout << "> Failed to receive Msg ...\n";
			closesocket(m_ClientSocket);
			return;
		}
		else if (nRecvLen == -1)
		{
			std::cout << "> Error : " << WSAGetLastError() << std::endl;
			closesocket(m_ClientSocket);
			return;
		}

		if (_tcsstr(szReceiveMsg, BUFFER_START) == szReceiveMsg)
			ReceivedMsgHandling(szReceiveMsg);
	}
}

VOID SLCPClient::ReceivedMsgHandling(const TCHAR* lpReceivedMsg)
{
	int code = static_cast<int>(0xff & ((CHAR*)lpReceivedMsg)[22]
		| (0xff & ((CHAR*)lpReceivedMsg)[23]) << 8
		| (0xff & ((CHAR*)lpReceivedMsg)[24]) << 16
		| (0xff & ((CHAR*)lpReceivedMsg)[25]) << 24);

	// Size definition will have to be modified
	TCHAR strContents[2035]; // BUFFER_SIZE - length of BUFFER_START, Code
	ZeroMemory(strContents, sizeof(strContents));
	//
	memcpy(strContents, lpReceivedMsg + 13, sizeof(strContents));
	TCHAR* lpFind = _tcsstr(strContents, BUFFER_END);
	int nFindIndex = static_cast<int>(lpFind - strContents);
	strContents[nFindIndex] = _T('\0');

	//std::wcout << "Receive contents {" << strContents << "}" << std::endl;

	switch (code)
	{
	case CHECK_STATUS_SUCCESS:
		std::cout << "> Status updated successfully ...\n";
		break;

	case CHECK_STATUS_FAIL:
		std::cout << "> Failed to update status ...\n";
		break;

	case POLICY_SEND:
	{
		SHARED_MEMORY_DATA tempSharedData;
		memcpy(&tempSharedData, &m_SharedData, sizeof(SHARED_MEMORY_DATA));

		TCHAR* lpSplit = contextsTok(strContents, _T(":,"));
		int nCnt = 0;
		while (lpSplit != NULL)
		{
			nCnt++;

			switch (nCnt)
			{
			case 2:
				memcpy(m_SharedData.logCreationPolicy, lpSplit, sizeof(lpSplit));
				break;
			case 4:
				memcpy(m_SharedData.backupFileStorage, lpSplit, sizeof(lpSplit));
				break;
			default:
				break;
			}

			lpSplit = contextsTok(NULL, _T(":,"));
		}
		std::cout << "> Policy data received ... {";
		std::wcout << "logCreationPolicy:" << m_SharedData.logCreationPolicy
			<< ", backupFileStorage:" << m_SharedData.backupFileStorage << "}" << std::endl;
		if (memcmp(&tempSharedData, &m_SharedData, sizeof(SHARED_MEMORY_DATA)))
		{
			m_Monitor.WriteSharedMem(m_SharedData);
			m_Monitor.SendEvent();
		}
		// if error occurred or unexpected data received,
		// SendMsg(POLICY_SEND_FAIL);
		// else,
		SendMsg(POLICY_SEND_SUCCESS);
		break;
	}

	case LOG_DATA_SEND_SUCCESS:
		std::wcout << "> Log data of {" << strContents << "} sent successfully ...\n";
		if (memcmp(m_SharedData.backupFileStorage, _T("1"), sizeof(_T("1"))))
		{
			if (LogFileDeletion(strContents) == false)
				std::cout << "> Fail to remove log file ...\n";
			else
				std::wcout << "> Log file {" << strContents << "} removed successfully ...\n";
		}
		else
		{
			if (LogFileBackup(strContents) == false)
				std::cout << "> Fail to move backup file ...\n";
			else
				std::wcout << "> Backup file {" << strContents << "} moved successfully ...\n";
		}
		break;

	case LOG_DATA_SEND_FAIL:
		std::wcout << "> Fail to send Log data of {" << strContents << "} ...\n";
		break;

	default:
		;
	}
}

VOID SLCPClient::SendMsg(int code)
{
	TCHAR szSendMsg[BUFFER_SIZE];
	unsigned char szByteCode[4];
	int nMsgLen = 0;

	ZeroMemory(szSendMsg, sizeof(szSendMsg));
	ZeroMemory(szByteCode, sizeof(szByteCode));

	szByteCode[0] = code & 0xFF;
	szByteCode[1] = (code >> 8) & 0xFF;
	szByteCode[2] = (code >> 16) & 0xFF;
	szByteCode[3] = (code >> 24) & 0xFF;

	memcpy(szSendMsg, BUFFER_START, sizeof(BUFFER_START) - 1);
	memcpy(szSendMsg + _tcslen(BUFFER_START), szByteCode, sizeof(TCHAR) * 2);
	memcpy(szSendMsg + _tcslen(BUFFER_START) + 2, BUFFER_END, sizeof(BUFFER_END) - 1);
	nMsgLen = _tcslen(BUFFER_START) + 2 + _tcslen(BUFFER_END);

	int nSendLen = send(m_ClientSocket, (char*)szSendMsg, sizeof(TCHAR) * nMsgLen, 0);

	if (nSendLen == SOCKET_ERROR)
	{
		std::cout << "> Failed to send Msg ...\n";
		return;
	}
}

VOID SLCPClient::SendMsg(int code, const TCHAR* lpFileName)
{
	TCHAR szSendMsg[BUFFER_SIZE];
	unsigned char szByteCode[4];
	int nMsgLen = 0;

	ZeroMemory(szSendMsg, sizeof(szSendMsg));
	ZeroMemory(szByteCode, sizeof(szByteCode));

	szByteCode[0] = code & 0xFF;
	szByteCode[1] = (code >> 8) & 0xFF;
	szByteCode[2] = (code >> 16) & 0xFF;
	szByteCode[3] = (code >> 24) & 0xFF;

	memcpy(szSendMsg, BUFFER_START, sizeof(BUFFER_START) - 1);
	memcpy(szSendMsg + _tcslen(BUFFER_START), szByteCode, sizeof(TCHAR) * 2);
	nMsgLen = _tcslen(BUFFER_START) + 2;

	// Size definition will have to be modified
	TCHAR strLogData[2026]; // BUFFER_SIZE - length of BUFFER_START, Code, BUFFER_END
	TCHAR strAppendMsg[2035]; // BUFFER_SIZE - length of BUFFER_START, Code
	ZeroMemory(strLogData, sizeof(strLogData));
	ZeroMemory(strAppendMsg, sizeof(strAppendMsg));
	//

	TCHAR strLogFile[MAX_PATH];
	ZeroMemory(strLogFile, sizeof(strLogFile));
	StringCbPrintf(strLogFile, sizeof(strLogFile), _T("%slog\\%s"), m_LogPath, lpFileName);

	FILE* fileHandle;
	int nLength = 0;
	_tfopen_s(&fileHandle, strLogFile, _T("rb"));
	fseek(fileHandle, 0, SEEK_END);
	nLength = ftell(fileHandle);
	rewind(fileHandle);
	fread(strLogData, 1, nLength, fileHandle);
	fclose(fileHandle);

	StringCbPrintf(strAppendMsg, sizeof(strAppendMsg),
		_T("Log File Name[%s]\n%s%s"), lpFileName, strLogData, BUFFER_END);
	std::wcout << "> " << strAppendMsg << std::endl;

	memcpy(szSendMsg + nMsgLen, strAppendMsg, sizeof(TCHAR) * _tcslen(strAppendMsg));
	nMsgLen += _tcslen(strAppendMsg);

	int nSendLen = send(m_ClientSocket, (char*)szSendMsg, sizeof(TCHAR) * nMsgLen, 0);

	if (nSendLen == SOCKET_ERROR)
	{
		std::cout << "> Failed to send Msg ...\n";
		return;
	}
}

BOOL SLCPClient::LogFileDeletion(const TCHAR* lpFile)
{
	TCHAR strLogFile[MAX_PATH];
	ZeroMemory(strLogFile, sizeof(strLogFile));
	StringCbPrintf(strLogFile, sizeof(strLogFile), _T("%slog\\%s"), m_LogPath, lpFile);

	if (_tremove(strLogFile) != 0)
		return false;

	return true;
}

BOOL SLCPClient::LogFileBackup(const TCHAR* lpFile)
{
	TCHAR strLogFile[MAX_PATH];
	TCHAR strBackupFile[MAX_PATH];
	ZeroMemory(strLogFile, sizeof(strLogFile));
	ZeroMemory(strBackupFile, sizeof(strBackupFile));
	StringCbPrintf(strLogFile, sizeof(strLogFile), _T("%slog\\%s"), m_LogPath, lpFile);

	StringCbPrintf(strBackupFile, sizeof(strBackupFile), _T("%sbackup"), m_LogPath);
	CreateDirectory(strBackupFile, NULL);
	StringCbPrintf(strBackupFile, sizeof(strBackupFile), _T("%sbackup\\%s"), m_LogPath, lpFile);

	if (!MoveFile(strLogFile, strBackupFile))
		return false;
	return true;
}

VOID SLCPClient::ExitProgram()
{
	CloseConnect();

	// Do something
}

