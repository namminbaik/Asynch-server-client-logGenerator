#include "pch.h"
#include "SLCPCommon.h"

TCHAR* contextsTok(TCHAR* lpstr1, const TCHAR* lpstr2)
{
	static _declspec(thread) TCHAR* sOrg = NULL;

	if (lpstr1)
		sOrg = lpstr1;
	else if (!sOrg)
		return NULL;
	else
		lpstr1 = sOrg;

	TCHAR* lpRetStr = sOrg;

	while (TCHAR chr = *lpstr1++)
	{
		for (int i = 0; lpstr2[i]; ++i)
		{
			if (chr == lpstr2[i])
			{
				lpstr1[-1] = 0;
				sOrg = lpstr1;
				return lpRetStr;
			}
		}
	}
	sOrg = NULL;
	return lpRetStr;
}