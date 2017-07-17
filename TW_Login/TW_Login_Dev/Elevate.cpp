// Elevate.cpp: implementation of the CElevate class.
//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Elevate.h"
#include <string>

#pragma warning(disable : 4996)

//////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////////

#define UNSERPPORTED_UAC 1			// UAC가 지원되지 않는 운영체제로 판단
#define FAILED_STATUS_SEARCH 2		// 상태 정보를 조회할 수 없음
#define NEEDED_ELEVATION 3			// Elevation이 필요함
#define ALREADY_ELEVATION 4			// 이미 Elevation이 완료됨

CElevate::CElevate()
{
}

CElevate::~CElevate()
{
}

int CElevate::GetNeedElevate()
{
	if (IsVistaOrHigher())
	{
		BOOL bResult = FALSE;

		if (SUCCEEDED(IsElevated(&bResult)))
		{
			if (bResult == TRUE)
				return ALREADY_ELEVATION;
			else
				return NEEDED_ELEVATION;
		}
		else
			return FAILED_STATUS_SEARCH;
	}
	
	return UNSERPPORTED_UAC;
}

BOOL CElevate::IsVistaOrHigher(void)
{
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (GetVersionEx(&versionInfo) && versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && versionInfo.dwMajorVersion >= 6)
		return TRUE;
	else
		return FALSE;
}

HRESULT CElevate::IsElevated(BOOL *pElevated)
{
	HRESULT hResult = E_FAIL;
	HANDLE hToken = NULL;

	if (!IsVistaOrHigher())
		return hResult;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return hResult;

	TOKEN_ELEVATION te = {0};
	DWORD dwReturnLength = 0;
	const int TokenElevation = 20;

	if (GetTokenInformation(hToken, (TOKEN_INFORMATION_CLASS)TokenElevation, &te, sizeof(te), &dwReturnLength))
	{
		hResult = te.TokenIsElevated ? S_OK : S_FALSE;

		if (pElevated)
			*pElevated = (te.TokenIsElevated != 0);
	}

	CloseHandle(hToken);
	
	return hResult;
}

BOOL CElevate::ShellExecWithVerb(HWND hWnd, LPCTSTR lpVerb, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory)
{
	SHELLEXECUTEINFO executeInfo;
	memset(&executeInfo, 0, sizeof(executeInfo));

	executeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	executeInfo.fMask = 0;
	executeInfo.hwnd = hWnd;
	executeInfo.lpVerb = lpVerb;
	executeInfo.lpFile = lpPath;
	executeInfo.lpParameters = lpParameters;
	executeInfo.lpDirectory = lpDirectory;
	executeInfo.nShow = SW_NORMAL;

	return ShellExecuteEx(&executeInfo);
}

BOOL CElevate::ShellExecWithElevation(HWND hWnd, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory)
{
	return ShellExecWithVerb(hWnd, TEXT("runas"), lpPath, lpParameters, lpDirectory);
}

BOOL CElevate::SelfExecuteWithElevation()
{
	TCHAR pszPathName[_MAX_PATH] = TEXT("");
	TCHAR lpParameter[1024] = TEXT("");

	if (__argc > 1)
	{
		for (int i = 1; i < __argc; i++)
		{
			_tcscat(lpParameter, __wargv[i]);
			_tcscat(lpParameter, TEXT(" "));
		}
	}

	::GetModuleFileName(::AfxGetInstanceHandle(), pszPathName, _MAX_PATH);

	// PathRemoveFileSpec(pszPathName);
	return ShellExecWithElevation(NULL, pszPathName, lpParameter, TEXT(""));
}

BOOL CElevate::OpenUrlWithElevation(HWND hWnd, LPCTSTR lpUrl)
{
	_TCHAR lpBuffer[MAX_PATH + 1];

	if (!SHGetSpecialFolderPath(hWnd, lpBuffer, CSIDL_PROGRAM_FILES, 0))
		return FALSE;

	_tcscat(lpBuffer, TEXT("\\Internet Explorer\\iexplorer.exe"));
	
	return ShellExecWithElevation(hWnd, lpBuffer, lpUrl, TEXT(""));
}