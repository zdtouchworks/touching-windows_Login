// Elevate.h: interface for the CElevate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEVATE_H__E7B50ED2_9C9D_4C00_93E9_C9547465DEFA__INCLUDED_)
#define AFX_ELEVATE_H__E7B50ED2_9C9D_4C00_93E9_C9547465DEFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Windows Vista 이상 OS 에서 관리자 권한 상승을 위한 Class
// 출처 : acidpop.Tistory.com
// 제작 : acidpop.Tistory.com
// 제작날짜 : 2012/02/28
// 수정 : ZD. (Touchworks Inc.)
// 수정날짜 : 2016/12/12

#include <shellapi.h>
#include <ShlObj.h>

//typedef struct _TOKEN_ELEVATION {
	//DWORD TokenIsElevated;
//} TOKEN_ELEVATION, *PTOKEN_ELEVATION;

#ifndef CSIDL_PROGRAM_FILES
#define CSIDL_PROGRAM_FILES 0x0026
#endif // CSIDL_PROGRAM_FILES

class CElevate
{
public:
	CElevate();
	virtual ~CElevate();

public:
	// 특정 프로그램을 관리자 권한으로 실행 하기 위한 함수
	BOOL ShellExecWithElevation(HWND hWnd, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);

	// 자기 자신을 관리자 권한으로 다시 시작하는 함수
	BOOL SelfExecuteWithElevation();

	// 특정 Url을 관리자 권한으로 Internet Explorer 를 실행하는 함수
	BOOL OpenUrlWithElevation(HWND hWnd, LPCTSTR lpUrl);

	// 권한 상승이 필요한지 여부, 1 - UAC 미지원 OS, 2 - 상태 정보 조회 불가, 3 - 권한 상승 필요, 4 - 권한 상승 되어 있음
	int GetNeedElevate();

	// OS가 Vista 보다 상위인지 여부
	BOOL IsVistaOrHigher(void);

	// 권한 체크
	HRESULT IsElevated(BOOL *pElevated);

private:
	// 관리자 권한으로 실행하기 위한 가장 기초 함수. ShellExecWithElevation 사용하도록 권장
	BOOL ShellExecWithVerb(HWND hWnd, LPCTSTR lpVerb, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);
};

#endif // !defined(AFX_ELEVATE_H__E7B50ED2_9C9D_4C00_93E9_C9547465DEFA__INCLUDED_)