#ifndef SLCP_COMMON_H
#define SLCP_COMMON_H

// Network communication
#define NETWORK_ADDRESS			_T("127.0.0.1")
#define NETWORK_PORT			10101
#define BUFFER_SIZE				2048
#define BUFFER_START			_T("|SLCPSTART|")
#define BUFFER_END				_T("|SLCPEND|")
#define STATUS_CHECK_INTERVAL	60	// Seconds
#define CHECK_STATUS			10
#define CHECK_STATUS_SUCCESS	11
#define CHECK_STATUS_FAIL		12
#define POLICY_REQUEST			20
#define POLICY_SEND				21
#define POLICY_SEND_SUCCESS		22
#define POLICY_SEND_FAIL		23
#define LOG_DATA_SEND			50
#define LOG_DATA_SEND_SUCCESS	51
#define LOG_DATA_SEND_FAIL		52

// Process communication
#define PROCESS_NAME			__T("SimpleLogGenerator.exe")
#define EVENT_NAME				__T("SLCPEvent")
#define SHARED_MEMORY_NAME		__T("SLCPSharedMemory")

typedef struct _SHARED_MEMORY_DATA
{
	TCHAR logCreationPolicy[2];	// 0: everything,	1: malware only
	TCHAR backupFileStorage[2];	// 0: Delete		1: Storage
}SHARED_MEMORY_DATA, *LPSHARED_MEMORY_DATA;

// Helping function
TCHAR* contextsTok(TCHAR* lpstr1, const TCHAR* lpstr2);
VOID ConsoleClosingComments(int interval);	// This is for console windows to terminate with 'X' button.

#endif // !SLCP_COMMON_H
