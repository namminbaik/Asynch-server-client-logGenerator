#ifndef SLCP_MONITOR_H
#define SLCP_MONITOR_H

#include "SLCPCommon.h"

class SLCPMonitor
{
public:
	SLCPMonitor();
	~SLCPMonitor();

	BOOL ConnectEvent();
	VOID SendEvent();
	BOOL CreateSharedMem();
	VOID WriteSharedMem(const SHARED_MEMORY_DATA& policy);

private:
	HANDLE m_hEvent;
	HANDLE m_hSharedMem;
};

#endif // !SLCP_MONITOR_H
