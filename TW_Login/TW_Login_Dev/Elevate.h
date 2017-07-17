// Elevate.h: interface for the CElevate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEVATE_H__E7B50ED2_9C9D_4C00_93E9_C9547465DEFA__INCLUDED_)
#define AFX_ELEVATE_H__E7B50ED2_9C9D_4C00_93E9_C9547465DEFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Windows Vista �̻� OS ���� ������ ���� ����� ���� Class
// ��ó : acidpop.Tistory.com
// ���� : acidpop.Tistory.com
// ���۳�¥ : 2012/02/28
// ���� : ZD. (Touchworks Inc.)
// ������¥ : 2016/12/12

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
	// Ư�� ���α׷��� ������ �������� ���� �ϱ� ���� �Լ�
	BOOL ShellExecWithElevation(HWND hWnd, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);

	// �ڱ� �ڽ��� ������ �������� �ٽ� �����ϴ� �Լ�
	BOOL SelfExecuteWithElevation();

	// Ư�� Url�� ������ �������� Internet Explorer �� �����ϴ� �Լ�
	BOOL OpenUrlWithElevation(HWND hWnd, LPCTSTR lpUrl);

	// ���� ����� �ʿ����� ����, 1 - UAC ������ OS, 2 - ���� ���� ��ȸ �Ұ�, 3 - ���� ��� �ʿ�, 4 - ���� ��� �Ǿ� ����
	int GetNeedElevate();

	// OS�� Vista ���� �������� ����
	BOOL IsVistaOrHigher(void);

	// ���� üũ
	HRESULT IsElevated(BOOL *pElevated);

private:
	// ������ �������� �����ϱ� ���� ���� ���� �Լ�. ShellExecWithElevation ����ϵ��� ����
	BOOL ShellExecWithVerb(HWND hWnd, LPCTSTR lpVerb, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);
};

#endif // !defined(AFX_ELEVATE_H__E7B50ED2_9C9D_4C00_93E9_C9547465DEFA__INCLUDED_)