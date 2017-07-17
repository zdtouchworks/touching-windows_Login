
// TW_LoginDlg.h : header file
//

#pragma once
#include <afxcmn.h>
#include "TW_LoginWnd.h"

#define SINGLE_STRING	0
#define ARRAY_STRING	1

//#define DEVELOPER 1

// CTW_LoginDlg dialog
class CTW_LoginDlg : public CDialog
{
// Construction
public:
	CTW_LoginDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TW_LOGIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	BOOL DrawRect(CPaintDC *dc);
	BOOL CreateItem(CPaintDC *dc);
	BOOL GetStringWidth(CPaintDC *dc);
	BOOL DrawImage(CPaintDC *dc);

	BOOL LoginInfoParsing(CString cstr_logininfo); 
	BOOL ReadAndDecrypt();
	CString SaveLoginInfoCreated(void);
	BOOL EncryptAndSavedLoginInfo(void);

	//int Encrypt(LPCTSTR lpszIn, LPSTR lpszOut, int nBufLen);
	//int Decrypt(LPCTSTR lpszIn, LPSTR lpszOut);
	//int Encrypt(void);
	//int Decrypt(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//afx_msg void OnBnClickedOpenWebBtn();
	afx_msg void OnStaticTopRemoteLink(void);
	afx_msg void OnStaticPwdStatus(void);
	afx_msg void OnBnClickedLogin(void);
	afx_msg void OnBnClickedDestroy(void);

	BOOL CreateNewBrowser();
	void NavigateWeb();

	DECLARE_MESSAGE_MAP()

public:

	IWebBrowser2*	m_pWebBrowser2;
#ifdef DEVELOPERS
	CRect			cr_URL_Text;
	CRect			cr_URL;
	CStatic			cStatic_URL_Text;
	CEdit			cEdit_URL;
	CSize			size_URL_Text;
#endif// RELEASE // Release
	CString			cStr_Url;
	CString			cStr_Remote_Url;
	
	CFont			NewFont[5];
	CFont*			pOldFont[5];
	CFont			pFont;

	CRect			cr_Client;
	//CRect			cr_Touching_Logo;
	CRect			cr_Top_Left_Status;
	CRect			cr_Top_Right_Status;
	CRect			cr_Top_Remote_Link;

	CRect			cr_LoginID_Text;
	CRect			cr_Password_Text;
	CRect			cr_LoginID;
	CRect			cr_Password;
	
	CRect			cr_Chk_AutoLogin;
	CRect			cr_Password_Status;

	CRect			cr_Bottom_CI;
	CRect			cr_Copyright;
	CRect			cr_Customer_Center_Text;
	CRect			cr_Customer_Center_Number;

	CRect			cr_Btn_OK;
	CRect			cr_Btn_Destroy;

	CStatic			cStatic_Top_Left_Status;
	CStatic			cStatic_Top_Right_Status;
	CStatic			cStatic_Top_Remote_Link;
	CStatic			cStatic_LoginID_Text;
	CStatic			cStatic_Password_Text;
	CStatic			cStatic_Password_Status;
	CStatic			cStatic_Bottom_BI;
	CStatic			cStatic_Copyright;
	CStatic			cStatic_Customer_Center_Text;
	CStatic			cStatic_Customer_Center_Number;
	CEdit			cEdit_LoginID;
	CEdit			cEdit_Password;
	CButton			cBtn_Chk_Save_Toggle;
	CButton			cBtn_OK;
	CButton			cBtn_Destroy;

	CString			cStr_Top_Left_Status;
	CString			cStr_Top_Right_Status;
	CString			cStr_Top_Remote_Link;

	CString			cStr_Touching_Logo;
	CString			cStr_LoginID_Text;
	CString			cStr_Password_Text;

	CString			cStr_Password_Status;

	CString			cStr_Bottom_CI;
	CString			cStr_Copyright;
	CString			cStr_Customer_Center_Text;
	CString			cStr_Customer_Center_Number;

	CSize			size_Top_Left_Status;
	CSize			size_Top_Right_Status;
	CSize			size_Top_Remote_Link;
	CSize			size_LoginID_Text;
	CSize			size_Password_Text;
	CSize			size_Chk_AutoLogin;
	CSize			size_Password_Status;
	CSize			size_Bottom_CI;
	CSize			size_Copyright;
	CSize			size_Customer_Center_Text;
	CSize			size_Customer_Center_Number;

	HDC				hDC;
	HINSTANCE		g_hInst;

	HRSRC			hResource_BI;
	DWORD			imageSize_BI;
	HGLOBAL			hGlobal_BI;
	LPVOID			pData_BI;
	HGLOBAL			hBuffer_BI;
	LPVOID			pBuffer_BI;
	IStream*		pStream_BI;
	HRESULT			hr_BI;

	HRSRC			hResource_CI;
	DWORD			imageSize_CI;
	HGLOBAL			hGlobal_CI;
	LPVOID			pData_CI;
	HGLOBAL			hBuffer_CI;
	LPVOID			pBuffer_Logo;
	IStream*		pStream_CI;
	HRESULT			hr_CI;

	CString*		cStr_LoginID;
	CString*		cStr_Password;
	CString*		cStr_SaveToggle;
//#endif // !DEVELOPERS
	int				Multi_Count;
	int				Card_Count;

	int				d_IE_Version;

private:
	CTW_LoginWnd m_LoginWnd;

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
