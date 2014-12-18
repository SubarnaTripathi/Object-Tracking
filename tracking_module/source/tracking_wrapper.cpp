# include <windows.h>
# include "WinInterface.h"
# include "global_ast_defines.h"

extern "C" DWORD WINAPI tracking_thread_wrapper(LPVOID);
extern "C" int tracking_thread(LPVOID);
extern "C" int ReleaseSemaphore_AST(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);
extern "C" unsigned int WaitForSingleObject_AST(HANDLE hHandle, DWORD dwMilliseconds);
extern "C" HANDLE CreateSemaphore_AST();

DWORD WINAPI tracking_thread_wrapper(LPVOID p)
{
	tracking_thread(p);
	return 0;
}

HANDLE CreateSemaphore_AST()
{
	return CreateSemaphore(NULL, 0, 1, NULL);	
}

int ReleaseSemaphore_AST(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount)
{
	ReleaseSemaphore(hSemaphore, lReleaseCount, lpPreviousCount);return 0;
}

unsigned int WaitForSingleObject_AST(HANDLE hHandle, DWORD dwMilliseconds)
{
	WaitForSingleObject(hHandle, dwMilliseconds);return 0;
}