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

	afx_msg void OnDocumentComplete(IDispatch *pDisp, VARIANT *URL); // �������� �ε� �Ϸ� �̺�Ʈ
	afx_msg void OnQuit(); // ����� �������� ���� �̺�Ʈ

private:
	BOOL bScriptLogin;
	DWORD m_dwCookie;
	IWebBrowser2* m_pWebBrowser2;
	CString cstr_Url, cstr_ID, cstr_Pwd, cstr_FormName, cstr_IDObjectID, cstr_PwdObjectID, cstr_ClickObjectID, cstr_ScriptName;

public:
	// �α��� �Լ�, ���� �α����� ��ư�� Ŭ���� ��ó�� ���� �ڵ��α��� �ǵ��� �Ѵ�.
	BOOL AutoLoginByButton(CString strUrl, CString strID, CString strPwd, CString strFormName, CString strIDObjectID, CString strPwdObjectID, CString strClickObjectID);

	// �α��� �Լ�, ���� �α����� �ڹٽ�ũ��Ʈ�� �����Ŵ���ν� �ڵ��α��� �ǵ��� �Ѵ�.
	BOOL AutoLoginByScript(CString strUrl, CString strID, CString strPwd, CString strFormName, CString strIDObjectID, CString strPwdObjectID, CString strClickObjectID);

protected:
	BOOL CreateNewBrowser(); // ���ο� ������ ����
	BOOL AdviseBrowser(); // �̺�Ʈ ��ũ
	BOOL UnAdviseBrowser(); // ��ũ ����
	void NavigateWeb(); // Ȩ������ Ž������

	BOOL PutText(CString strFormName, CString strObjectID, CString strText);
	BOOL LoginByButton(); // ��ư�� Ŭ���� ��ó�� �Ͽ� �ڵ��α��� �ǵ��� �Ѵ�.
	BOOL LoginByScript(); // ��ư�� Ŭ���� ��ó�� �Ͽ� �ڵ��α��� �ǵ��� �Ѵ�.
public:

};