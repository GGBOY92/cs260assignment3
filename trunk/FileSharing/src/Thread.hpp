
/*!
 *   @File   ActiveObject.hpp
 *   @Author Steven Liss and Westley Hennigh
 *   @Date   25 Feb 2010, 20 Oct 2010
 *   @Brief  Wrapper for win32 threads.
 */

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Thread
{
public:
	Thread ( DWORD (WINAPI* pFun) (void* arg), void* pArg )
	{
		thread_handle = CreateThread (
			0,  // Security attributes
			0,  // Stack size
			pFun,
			pArg,
			CREATE_SUSPENDED,  // Start in suspended state, resume later
			&id );
	}
	~Thread () { CloseHandle (thread_handle); }

	int Resume () { return ResumeThread (thread_handle); }

	void WaitForDeath ()
	{
		WaitForSingleObject (thread_handle, INFINITE);  // Wait forever for the thread to close...
	}
private:
	HANDLE thread_handle;
	DWORD  id;
};
