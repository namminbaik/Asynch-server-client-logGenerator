#include "pch.h"
#include "SLCPMonitor.h"

SLCPMonitor::SLCPMonitor()
	: m_hEvent{ NULL }, m_hSharedMem{ NULL }
{
}

SLCPMonitor::~SLCPMonitor()
{
	if (m_hEvent != NULL)
		CloseHandle(m_hEvent);

	if (m_hSharedMem != NULL)
		CloseHandle(m_hSharedMem);
}

BOOL SLCPMonitor::ConnectEvent()
{
	m_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME);
	if (m_hEvent == NULL)
		return false;

	return true;
}

VOID SLCPMonitor::SendEvent()
{
	SetEvent(m_hEvent);
}

BOOL SLCPMonitor::CreateSharedMem()
{
	m_hSharedMem = CreateFileMapping(
		INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SHARED_MEMORY_DATA), SHARED_MEMORY_NAME);
	if (m_hSharedMem == NULL)
		return false;

	return true;
}

VOID SLCPMonitor::WriteSharedMem(const SHARED_MEMORY_DATA& policy)
{
	LPCTSTR lpMapBuf = (LPTSTR)MapViewOfFile(
		m_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHARED_MEMORY_DATA));
	if (lpMapBuf == NULL)
	{
		std::cout << "> Failed to Write shared memory ...\n";
	}
	else
	{
		memcpy((PVOID)lpMapBuf, &policy, sizeof(policy));
		UnmapViewOfFile(lpMapBuf);
	}
}