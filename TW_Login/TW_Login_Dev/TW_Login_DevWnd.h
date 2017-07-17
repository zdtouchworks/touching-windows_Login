#pragma once

// CTW_LoginWnd
#include <afxctl.h>
#include <afxhtml.h>

class CTW_Login_DevWnd : public CWnd
{
	DECLARE_DYNAMIC(CTW_Login_DevWnd)

public:
	CTW_Login_DevWnd();
	virtual ~CTW_Login_DevWnd();

protected:
	DECLARE_MESSAGE_MAP()

	DECLARE_DISPATCH_MAP();

	afx_msg void OnDocumentComplete(IDispatch *pDisp, VARIANT *URL); // 웹피이지 로딩 완료 이벤트
	afx_msg void OnQuit(); // 실행된 웹피이지 종료 이벤트

private:
	BOOL bScriptLogin;
	DWORD m_dwCookie;
	IWebBrowser2* m_pWebBrowser2;
	CString cstr_Url, cstr_ID, cstr_Pwd, cstr_FormName, cstr_IDObjectID, cstr_PwdObjectID, cstr_ClickObjectID, cstr_ScriptName;

public:
	// 로그인 함수, 최종 로그인은 버튼을 클릭한 것처럼 만들어서 자동로그인 되도록 한다.
	BOOL AutoLoginByButton(CString strUrl, CString strID, CString strPwd, CString strFormName, CString strIDObjectID, CString strPwdObjectID, CString strClickObjectID);

	// 로그인 함수, 최종 로그인은 자바스크립트를 실행시킴으로써 자동로그인 되도록 한다.
	BOOL AutoLoginByScript(CString strUrl, CString strID, CString strPwd, CString strFormName, CString strIDObjectID, CString strPwdObjectID, CString strClickObjectID);

protected:
	BOOL CreateNewBrowser(); // 새로운 브라우저 생성
	BOOL AdviseBrowser(); // 이벤트 싱크
	BOOL UnAdviseBrowser(); // 싱크 해제
	void NavigateWeb(); // 홈페이지 탐색시작

	BOOL PutText(CString strFormName, CString strObjectID, CString strText);
	BOOL LoginByButton(); // 버튼을 클릭한 것처럼 하여 자동로그인 되도록 한다.
	BOOL LoginByScript(); // 버튼을 클릭한 것처럼 하여 자동로그인 되도록 한다.
public:

};