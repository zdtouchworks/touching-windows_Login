
// TW_LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TW_Login.h"
#include "TW_LoginDlg.h"
#include "afxdialogex.h"
#include "AESEncAndDec.h"
#include <fstream>
#include <iostream>

#include "../lib/rapidjson//document.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4996)

#define MAX_SIZE 2048
#define SAVE_LOGININFO "1"
#define NOSAVE_LOGININFO "0"



// -- zd. �������� �޾ƿ��� �ɷ� ����.
// -- zd. ����� 10�� ���� �ۿ� �ȵ�. ���� �ʿ�.
unsigned char szKey[11] = {"TW_AES_Enc"};

// CTW_LoginDlg dialog

using namespace std;

CTW_LoginDlg::CTW_LoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTW_LoginDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTW_LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTW_LoginDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_STATIC_TOP_REMOTE_LINK, OnStaticTopRemoteLink)
	ON_BN_CLICKED(IDC_STATIC_PWD_STATUS, OnStaticPwdStatus)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN_OK, OnBnClickedLogin)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN_DESTROY, OnBnClickedDestroy)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTW_LoginDlg message handlers

BOOL CTW_LoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	SetWindowText(TEXT("��Ī�Ŵ��� �α���"));

	m_pWebBrowser2 = NULL;
	
	this->SetWindowPos(NULL, 0, 0, 561, 319, SWP_NOMOVE | SWP_NOZORDER);
	
	CPaintDC dc(this);

	hDC = ::GetDC(m_hWnd);

	hResource_BI = FindResource(g_hInst, MAKEINTRESOURCE(IDB_TOUCHING_BI), TEXT("PNG"));
	hResource_CI = FindResource(g_hInst, MAKEINTRESOURCE(IDB_TOUCHWORKS_CI), TEXT("PNG"));

	//cStr_Url = TEXT("http://demo.touchinga.com");
	cStr_Url = TEXT("http://www.touchingpos.com");
	cStr_Remote_Url = TEXT("http://939.co.kr/touching");

	d_IE_Version = ::GetIEVersion();
	
	ReadAndDecrypt();

	HWND foregroundWindow = ::GetForegroundWindow();

	if (foregroundWindow == m_hWnd)
	{
		return TRUE;
	}

	if (::SetForegroundWindow(m_hWnd))
	{
		return TRUE;
	}

	DWORD foregroundThread = ::GetWindowThreadProcessId(foregroundWindow, NULL);
	DWORD myThread = ::GetWindowThreadProcessId(m_hWnd, NULL);
	::AttachThreadInput(foregroundThread, myThread, true);
	::SetForegroundWindow(m_hWnd);
	::AttachThreadInput(foregroundThread, myThread, false);

	//GetDlgItem(IDD_TW_LOGIN_DIALOG)->MoveWindow(100, 100, 661, 419);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTW_LoginDlg::OnPaint()
{
	GetClientRect(cr_Client);

	CPaintDC dc(this);
	GetStringWidth(&dc);
	DrawRect(&dc);
	CreateItem(&dc);
	DrawImage(&dc);	
	
	if (IsIconic())
	{
		//CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		
	}
	else
	{
		cEdit_LoginID.SetFocus();
		cEdit_LoginID.SetSel(cEdit_LoginID.GetWindowTextLengthW(), cEdit_LoginID.GetWindowTextLengthW());

		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTW_LoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CTW_LoginDlg::DrawRect(CPaintDC *dc)
{
#ifdef DEVELOPERS
	cr_URL = CRect(cr_Client.right - 10 - 230, 15, cr_Client.right - 50, 35);
	cr_URL_Text = CRect(cr_URL.left - 7 - size_URL_Text.cx, 15, cr_URL.left - 7, 35);
#endif // !DEVELOPERS

	cr_Top_Left_Status = CRect(15, 70, 15 + size_Top_Left_Status.cx, 70 + size_Top_Left_Status.cy);
	cr_Top_Right_Status = CRect(cr_Client.right - 15 - size_Top_Right_Status.cx, cr_Top_Left_Status.top, cr_Client.right - 15, cr_Top_Left_Status.bottom);
	cr_Top_Remote_Link = CRect(cr_Top_Left_Status.right, cr_Top_Left_Status.top, cr_Top_Right_Status.left, cr_Top_Left_Status.bottom);

	cr_LoginID_Text = CRect(140, cr_Top_Left_Status.bottom + 20, 140 + size_LoginID_Text.cx, cr_Top_Left_Status.bottom + 20 + size_LoginID_Text.cy);
	cr_Password_Text = CRect(cr_LoginID_Text.left, cr_LoginID_Text.bottom + 20, cr_LoginID_Text.left + size_LoginID_Text.cx, cr_LoginID_Text.bottom + 20 + size_LoginID_Text.cy);

	cr_LoginID = CRect(cr_LoginID_Text.right + 30, cr_LoginID_Text.top -3, cr_Client.right - 140, cr_LoginID_Text.top + 3 + size_LoginID_Text.cy);
	cr_Password = CRect(cr_Password_Text.right + 30, cr_Password_Text.top - 3, cr_Client.right - 140, cr_Password_Text.top + 3 + size_LoginID_Text.cy);

	cr_Chk_AutoLogin = CRect(cr_LoginID_Text.left, cr_Password_Text.bottom + 15, cr_LoginID_Text.left + 20 + size_Chk_AutoLogin.cx, cr_Password_Text.bottom + 15 + size_Chk_AutoLogin.cy);
	cr_Password_Status = CRect(cr_Password.right - size_Password_Status.cx, cr_Chk_AutoLogin.top, cr_Password.right, cr_Chk_AutoLogin.bottom);

	cr_Btn_OK = CRect(60, cr_Chk_AutoLogin.bottom + 15, cr_Client.right / 2 - 20, cr_Chk_AutoLogin.bottom + 15 + 30);
	cr_Btn_Destroy = CRect(cr_Client.right / 2 + 20, cr_Btn_OK.top, cr_Client.right - 60, cr_Btn_OK.bottom);

	cr_Bottom_CI = CRect(15, cr_Client.bottom - 15 - size_Bottom_CI.cy, 15 + size_Bottom_CI.cx, cr_Client.bottom - 15);
	cr_Copyright = CRect((cr_Client.right / 2) - (size_Copyright.cx / 2), cr_Bottom_CI.top, (cr_Client.right / 2) + (size_Copyright.cx / 2), cr_Bottom_CI.bottom);
	cr_Customer_Center_Number = CRect(cr_Client.right - 15 - size_Customer_Center_Number.cx, cr_Bottom_CI.bottom - size_Customer_Center_Number.cy, cr_Client.right - 15, cr_Bottom_CI.bottom);
	cr_Customer_Center_Text = CRect(cr_Customer_Center_Number.left - 5 - size_Customer_Center_Text.cx, cr_Bottom_CI.top, cr_Customer_Center_Number.left - 5, cr_Bottom_CI.bottom);

	return TRUE;
}

BOOL CTW_LoginDlg::CreateItem(CPaintDC *dc)
{
#ifdef DEVELOPERS
	cStatic_URL_Text.Create(TEXT("http://"), WS_CHILD | WS_VISIBLE | ES_LEFT, cr_URL_Text, this, IDC_STATIC_URL_TEXT);
	cEdit_URL.Create(WS_CHILD | WS_VISIBLE | ES_LEFT | WS_TABSTOP | ES_AUTOHSCROLL, cr_URL, this, IDC_EDIT_URL);
	cStatic_URL_Text.SetFont(&NewFont[4], TRUE);
	cEdit_URL.SetWindowTextW(cStr_Url);
	//MessageBox(cStr_Url);
#endif // !DEVELOPERS

	pOldFont[1] = dc->SelectObject(&NewFont[1]);

	cStatic_Top_Left_Status.Create(cStr_Top_Left_Status, WS_CHILD | WS_VISIBLE | ES_LEFT, cr_Top_Left_Status, this, IDC_STATIC_TOP_LEFT_STATUS);
	cStatic_Top_Remote_Link.Create(cStr_Top_Remote_Link, WS_CHILD | WS_VISIBLE | SS_NOTIFY | ES_CENTER, cr_Top_Remote_Link, this, IDC_STATIC_TOP_REMOTE_LINK);
	cStatic_Top_Right_Status.Create(cStr_Top_Right_Status, WS_CHILD | WS_VISIBLE | ES_LEFT, cr_Top_Right_Status, this, IDC_STATIC_TOP_RIGHT_STATUS);

	cStatic_Top_Left_Status.SetFont(&NewFont[1], TRUE);
	cStatic_Top_Remote_Link.SetFont(&NewFont[2], TRUE);
	cStatic_Top_Right_Status.SetFont(&NewFont[1], TRUE);

	cStatic_LoginID_Text.Create(cStr_LoginID_Text, WS_CHILD | WS_VISIBLE | ES_LEFT, cr_LoginID_Text, this, IDC_STATIC_POSID_TEXT);
	cStatic_Password_Text.Create(cStr_Password_Text, WS_CHILD | WS_VISIBLE | ES_LEFT, cr_Password_Text, this, IDC_STATIC_PWD_TEXT);

	cStatic_LoginID_Text.SetFont(&NewFont[3], TRUE);
	cStatic_Password_Text.SetFont(&NewFont[3], TRUE);

	cEdit_LoginID.Create(WS_CHILD | WS_VISIBLE | ES_LEFT | WS_TABSTOP, cr_LoginID, this, IDC_EDIT_LOGIN_POSID);
	cEdit_Password.Create(WS_CHILD | WS_VISIBLE | ES_LEFT | WS_TABSTOP | ES_PASSWORD, cr_Password, this, IDC_EDIT_LOGIN_PWD);

	//cEdit_PosID.SetWindowTextW(TEXT("gongcha316"));
	//cEdit_Pwd.SetWindowTextW(TEXT("17041"));

	if (cStr_SaveToggle[0] == SAVE_LOGININFO)
	{
		cEdit_LoginID.SetWindowTextW(cStr_LoginID[0]);
		cEdit_Password.SetWindowTextW(cStr_Password[0]);
	}

	cBtn_Chk_Save_Toggle.Create(TEXT("�� ��"), BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | ES_LEFT | WS_TABSTOP, cr_Chk_AutoLogin, this, IDC_BUTTON_CHK_AUTOLOGIN);
	cBtn_Chk_Save_Toggle.SetCheck(BST_CHECKED);
	cStatic_Password_Status.Create(cStr_Password_Status, WS_CHILD | WS_VISIBLE | SS_NOTIFY | ES_LEFT, cr_Password_Status, this, IDC_STATIC_PWD_STATUS);

	cBtn_Chk_Save_Toggle.SetFont(&NewFont[3], TRUE);
	cStatic_Password_Status.SetFont(&NewFont[3], TRUE);
	
	cBtn_OK.Create(TEXT("�α���"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, cr_Btn_OK, this, IDC_BUTTON_LOGIN_OK);
	cBtn_Destroy.Create(TEXT("�� ��"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, cr_Btn_Destroy, this, IDC_BUTTON_LOGIN_DESTROY);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lstrcpy(lf.lfFaceName, TEXT("���� ���"));
	//lf.lfHeight = 20;
	lf.lfWeight = FW_BOLD;
	pFont.CreateFontIndirectW(&lf);
	cBtn_OK.SetFont(&pFont);
	cBtn_Destroy.SetFont(&pFont);

	//cStatic_Bottom_BI.Create(cStr_Bottom_BI, WS_CHILD | WS_VISIBLE | ES_LEFT, cr_Bottom_BI, this, IDC_STATIC_BOTTOM_BI);
	cStatic_Copyright.Create(cStr_Copyright, WS_CHILD | WS_VISIBLE | ES_CENTER, cr_Copyright, this, IDC_STATIC_COPYRIGHT);
	cStatic_Customer_Center_Number.Create(cStr_Customer_Center_Number, WS_CHILD | WS_VISIBLE | ES_LEFT, cr_Customer_Center_Number, this, IDC_STATIC_CUSTOMER_CENTER_NUMBER);
	cStatic_Customer_Center_Text.Create(cStr_Customer_Center_Text, WS_CHILD | WS_VISIBLE | ES_LEFT, cr_Customer_Center_Text, this, IDC_STATIC_CUSTOMER_CENTER_TEXT);

	//cStatic_Bottom_BI.SetFont(&NewFont[1], TRUE);
	cStatic_Copyright.SetFont(&NewFont[1], TRUE);
	cStatic_Customer_Center_Number.SetFont(&NewFont[4], TRUE);
	cStatic_Customer_Center_Text.SetFont(&NewFont[1], TRUE);

	return TRUE;
}

BOOL CTW_LoginDlg::GetStringWidth(CPaintDC *dc)
{
	NewFont[0].CreateFontW(28 * 1.3, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));

	NewFont[1].CreateFontW(12 * 1.3, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("���� ���"));

	//NewFont[2].CreateFontW(12 * 1.3, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("���� ���"));
	LOGFONT log;
	GetFont()->GetLogFont(&log);
	log.lfUnderline = TRUE;
	NewFont[2].CreateFontIndirectW(&log);

	NewFont[3].CreateFontW(15 * 1.3, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("���� ���"));

	NewFont[4].CreateFontW(15 * 1.3, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("���� ���"));

	cStr_Top_Left_Status = TEXT("��Ī �̿� �� ������ ����� ������ (1661-9097)�� ��ȭ�Ͻ� ��, ");
	cStr_Top_Remote_Link = TEXT("��������");
	cStr_Top_Right_Status = TEXT(" ��ư�� �����ּ���.");

	cStr_LoginID_Text = TEXT("���� ���̵�");
	cStr_Password_Text = TEXT("��й�ȣ");
	cStr_Password_Status = TEXT("��й�ȣ�� �����̳���?");

	cStr_Bottom_CI = TEXT("TouchWorks");
	cStr_Copyright = TEXT("Copyright �� Touchworks All right reserved.");
	cStr_Customer_Center_Text = TEXT("������");
	cStr_Customer_Center_Number = TEXT("1661-9097");

	pOldFont[1] = dc->SelectObject(&NewFont[1]);

	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Top_Left_Status, cStr_Top_Left_Status.GetLength(), &size_Top_Left_Status);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Top_Remote_Link, cStr_Top_Remote_Link.GetLength(), &size_Top_Remote_Link);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Top_Right_Status, cStr_Top_Right_Status.GetLength(), &size_Top_Right_Status);

	pOldFont[3] = dc->SelectObject(&NewFont[3]);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_LoginID_Text, cStr_LoginID_Text.GetLength(), &size_LoginID_Text);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Password_Text, cStr_Password_Text.GetLength(), &size_Password_Text);
	GetTextExtentPoint32(dc->GetSafeHdc(), TEXT("�ڵ� �α���"), lstrlen(TEXT("�ڵ� �α���")), &size_Chk_AutoLogin);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Password_Status, cStr_Password_Status.GetLength(), &size_Password_Status);

	pOldFont[1] = dc->SelectObject(&NewFont[1]);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Bottom_CI, cStr_Bottom_CI.GetLength(), &size_Bottom_CI);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Copyright, cStr_Copyright.GetLength(), &size_Copyright);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Customer_Center_Text, cStr_Customer_Center_Text.GetLength(), &size_Customer_Center_Text);

	pOldFont[4] = dc->SelectObject(&NewFont[4]);
	GetTextExtentPoint32(dc->GetSafeHdc(), cStr_Customer_Center_Number, cStr_Customer_Center_Number.GetLength(), &size_Customer_Center_Number);

#ifdef DEVELOPERS
	GetTextExtentPoint32(dc->GetSafeHdc(), TEXT("http://"), lstrlen(TEXT("http://")), &size_URL_Text);
#endif // !DEVELOPERS

	return TRUE;
}

Bitmap *pBitmap;
BOOL CTW_LoginDlg::DrawImage(CPaintDC *dc)
{
	///*
	Graphics graphics(hDC);

	imageSize_BI = SizeofResource(g_hInst, hResource_BI);
	hGlobal_BI = LoadResource(g_hInst, hResource_BI);
	pData_BI = LockResource(hGlobal_BI);
	hBuffer_BI = GlobalAlloc(GMEM_MOVEABLE, imageSize_BI);
	pBuffer_BI = GlobalLock(hBuffer_BI);
	CopyMemory(pBuffer_BI, pData_BI, imageSize_BI);
	GlobalUnlock(hBuffer_BI);
	hr_BI = CreateStreamOnHGlobal(hBuffer_BI, TRUE, &pStream_BI);

	Image imagePNG_BI(pStream_BI);

	pStream_BI->Release();
	if (imagePNG_BI.GetLastStatus() != Ok)
	{

	}

	imageSize_CI = SizeofResource(g_hInst, hResource_CI);
	hGlobal_CI = LoadResource(g_hInst, hResource_CI);
	pData_CI = LockResource(hGlobal_CI);
	hBuffer_CI = GlobalAlloc(GMEM_MOVEABLE, imageSize_CI);
	hBuffer_CI = GlobalLock(hBuffer_CI);
	CopyMemory(hBuffer_CI, pData_CI, imageSize_CI);
	GlobalUnlock(hBuffer_CI);
	hr_CI = CreateStreamOnHGlobal(hBuffer_CI, TRUE, &pStream_CI);

	Image imagePNG_CI(pStream_CI);

	pStream_CI->Release();
	if (imagePNG_CI.GetLastStatus() != Ok)
	{

	}

	//graphics.DrawImage(&imagePNG_CI, cr_Bottom_CI.left, cr_Bottom_CI.top, cr_Bottom_CI.right, cr_Bottom_CI.bottom);
	//graphics.DrawImage(&imagePNG_CI, cr_Copyright.left, cr_Copyright.top, cr_Copyright.right, cr_Copyright.bottom);
#ifdef DEVELOPERS
	graphics.DrawImage(&imagePNG_BI, ((cr_Client.right / 2) - ((imagePNG_BI.GetWidth() * 0.07) / 2)) / 2, 7, imagePNG_BI.GetWidth() * 0.07, imagePNG_BI.GetHeight() * 0.07);
#else
	graphics.DrawImage(&imagePNG_BI, (cr_Client.right / 2) - ((imagePNG_BI.GetWidth() * 0.07) / 2), 7, imagePNG_BI.GetWidth() * 0.07, imagePNG_BI.GetHeight() * 0.07);
#endif // !DEVELOPERS
	graphics.DrawImage(&imagePNG_CI, 15, cr_Client.bottom - 15 - (imagePNG_CI.GetHeight() * 0.15), imagePNG_CI.GetWidth() * 0.15, imagePNG_CI.GetHeight() * 0.15);

	//*/

	/*
	CBrush brush, *pOldBrush;
	brush.CreateSolidBrush(RGB(136, 525, 245));
	pOldBrush = dc->SelectObject(&brush);
	dc->Rectangle(cr_Bottom_CI);
	*/


	return TRUE;
}

BOOL CTW_LoginDlg::EncryptAndSavedLoginInfo(void)
{
	//Encrypt();

	if (IsDlgButtonChecked(IDC_BUTTON_CHK_AUTOLOGIN))
	{
		int nKeyLen = 10;
		unsigned char chPadding = 254;
		int nPaddingType = 0;

		CString cStr_Buffer = SaveLoginInfoCreated();
	
		unsigned char *szSrc = new unsigned char[cStr_Buffer.GetLength()];
		// zd. CString�� �޾ƿ� ���ڿ��� unsigned char* ���ۿ� ����.
		strcpy((char*)szSrc, (CStringA)cStr_Buffer);

		int nSrcLen = cStr_Buffer.GetLength() + 1;

		unsigned char *pszEncrypted = NULL;
		unsigned char *pszPlain		= NULL;

		unsigned long dwLen = AdjustSourceLen(nSrcLen, SOURCE_MULTIPLE);

		pszEncrypted = (unsigned char*)LocalAlloc(LPTR, dwLen * 2 + 1);

		ByteFillChar(pszEncrypted, chPadding, dwLen * 2);
		Encrypt(szKey, nKeyLen, chPadding, nPaddingType, szSrc, dwLen, (char*)pszEncrypted);

		std::ofstream out("C:\\Touchworks\\Touchworks.DAT");
		std::string str;

		if (out.is_open())
		{
			out << pszEncrypted << std::endl;;
			out.close();
		}

		//Decrypt(szKey, nKeyLen, chPadding, nPaddingType, pszEncrypted, lstrlenA((const char*)pszEncrypted), (char*)pszPlain);
				
		USES_CONVERSION;
		CString strTemp;
		strTemp = A2W((char*)pszEncrypted);

		LoginInfoParsing(cStr_Buffer);

		//MessageBox(strTemp);

		LocalFree(pszEncrypted);
		LocalFree(pszPlain);
	}

	return TRUE;
}

BOOL CTW_LoginDlg::PreTranslateMessage(MSG* pMsg)
{
	int xPos = GET_X_LPARAM(pMsg->lParam);
	int yPos = GET_Y_LPARAM(pMsg->lParam);

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			
			// focus�� edit id, edit pass, ok�� ���� ����.
			// 1. id ����� �� messagebox, 2. pass ����� �� messagebox, 3. else ��� Ȯ�� ��ư Ŭ�� ȿ��.
			CString cStr_PosID = TEXT("");
			CString cStr_Pwd = TEXT("");

			cEdit_LoginID.GetWindowTextW(cStr_PosID);
			cEdit_Password.GetWindowTextW(cStr_Pwd);

			if (cStr_PosID != TEXT("") && cStr_Pwd != TEXT(""))
			{
				CString str_PosID = TEXT("");
				CString str_Pwd = TEXT("");
				
				cEdit_LoginID.GetWindowTextW(str_PosID);
				cEdit_Password.GetWindowTextW(str_Pwd);

#ifdef DEVELOPERS
				cEdit_URL.GetWindowTextW(cStr_Url);
				cStr_Url = TEXT("http://") + cStr_Url;
#endif
				m_LoginWnd.AutoLoginByButton(cStr_Url, str_PosID, str_Pwd, TEXT("new_user"), TEXT("user[login_id]"), TEXT("user[password]"), TEXT("commit"));

				if (IsDlgButtonChecked(IDC_BUTTON_CHK_AUTOLOGIN))
				{
					EncryptAndSavedLoginInfo();
				}
			}
			else if (cStr_PosID == TEXT(""))
			{
				MessageBox(TEXT("���̵� �Է����ּ���."));

			}
			else if (cStr_Pwd == TEXT(""))
			{
				MessageBox(TEXT("��й�ȣ�� �Է����ּ���."));
			}


			return TRUE;
			
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH CTW_LoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	switch(nCtlColor)
	{
	case CTLCOLOR_STATIC:
		{
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_TOP_REMOTE_LINK)
			{
				pDC->SetTextColor(RGB(0, 0, 255));
				pDC->SetBkMode(TRANSPARENT);
				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}

			if (pWnd->GetDlgCtrlID() == IDC_STATIC_PWD_STATUS)
			{
				pDC->SetTextColor(RGB(0, 0, 255));
				pDC->SetBkMode(TRANSPARENT);
				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


BOOL CTW_LoginDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	CPoint pt;
	CRect rt;
	GetCursorPos(&pt);
	GetDlgItem(IDC_STATIC_TOP_REMOTE_LINK)->GetWindowRect(&rt);
	if (rt.PtInRect(pt))
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_HAND)));
		return TRUE;
	}

	GetDlgItem(IDC_STATIC_PWD_STATUS)->GetWindowRect(&rt);
	if (rt.PtInRect(pt))
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_HAND)));
		return TRUE;
	}

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CTW_LoginDlg::OnBnClickedLogin(void)
{
	CString strPosID(TEXT(""));
	CString strPwd(TEXT(""));

	cEdit_LoginID.GetWindowTextW(strPosID);
	cEdit_Password.GetWindowTextW(strPwd);

	if (!strPosID.IsEmpty() && !strPwd.IsEmpty())
	{
		m_LoginWnd.AutoLoginByButton(cStr_Url, strPosID, strPwd, TEXT("new_user"), TEXT("user[login_id]"), TEXT("user[password]"), TEXT("commit"));

		if (IsDlgButtonChecked(IDC_BUTTON_CHK_AUTOLOGIN))
		{
			EncryptAndSavedLoginInfo();
		}
	}
	else if (strPosID.IsEmpty())
	{
		MessageBox(TEXT("���̵� �Է����ּ���."));
	}
	else if (strPwd.IsEmpty())
	{
		MessageBox(TEXT("��й�ȣ�� �Է����ּ���."));
	}
}

void CTW_LoginDlg::OnBnClickedDestroy(void)
{
	m_nFlags = 1;
	PostMessage(WM_CLOSE);
}

void CTW_LoginDlg::OnStaticTopRemoteLink(void)
{
	GetDlgItem(IDC_STATIC_TOP_REMOTE_LINK)->Invalidate();
	CreateNewBrowser();
	//ShellExecute(m_hWnd, TEXT("open"), TEXT("http://939.co.kr/touching"), NULL, NULL, SW_SHOW);
}

void CTW_LoginDlg::OnStaticPwdStatus(void)
{
	GetDlgItem(IDC_STATIC_PWD_STATUS)->Invalidate();
	MessageBox(TEXT("�� ������ �����Դϴ�. 1661-9097�� �����ּ���."));
}

BOOL CTW_LoginDlg::CreateNewBrowser()
{
	HRESULT hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser2);

	if (SUCCEEDED(hr) && m_pWebBrowser2)
	{
		hr = m_pWebBrowser2->put_StatusBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_ToolBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_MenuBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_Visible(VARIANT_TRUE);

		NavigateWeb(); // �� Ž��
	}
	else
		return FALSE;

	return TRUE;
}


void CTW_LoginDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CString strPosID(TEXT(""));
	CString strPwd(TEXT(""));

	cEdit_LoginID.GetWindowTextW(strPosID);
	cEdit_Password.GetWindowTextW(strPwd);

	if (!strPosID.IsEmpty() && !strPwd.IsEmpty())
	{
		m_LoginWnd.AutoLoginByButton(cStr_Url, strPosID, strPwd, TEXT("new_user"), TEXT("user[login_id]"), TEXT("user[password]"), TEXT("commit"));

		if (IsDlgButtonChecked(IDC_BUTTON_CHK_AUTOLOGIN))
		{
			EncryptAndSavedLoginInfo();
		}
	}
	else if (strPosID.IsEmpty())
	{
		MessageBox(TEXT("���̵� �Է����ּ���."));
	}
	else if (strPwd.IsEmpty())
	{
		MessageBox(TEXT("��й�ȣ�� �Է����ּ���."));
	}

	//CDialog::OnOK();
}


void CTW_LoginDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}


// �� Ž��
void CTW_LoginDlg::NavigateWeb()
{
	COleVariant vUrl(cStr_Remote_Url, VT_BSTR), vFlags(long(navNoHistory | navNoReadFromCache | navNoWriteToCache), VT_I4), vNull(LPCTSTR(NULL), VT_BSTR);
	
	m_pWebBrowser2->Navigate2(&vUrl, &vFlags, &vNull, &vNull, &vNull);
	
	::SetWindowPos(m_hWnd, wndBottom, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	
	vUrl.Clear();
	vFlags.Clear();
	vNull.Clear();
}

CString CTW_LoginDlg::SaveLoginInfoCreated(void)
{
	CString cStr_LoginID_Buffer = TEXT("");
	CString cStr_Password_Buffer = TEXT("");
	CString cStr_SaveToggle_Buffer = 0;

	cEdit_LoginID.GetWindowTextW(cStr_LoginID_Buffer);
	cEdit_Password.GetWindowTextW(cStr_Password_Buffer);

	if (IsDlgButtonChecked(IDC_BUTTON_CHK_AUTOLOGIN))
	{
		cStr_SaveToggle_Buffer = SAVE_LOGININFO;
	}
	else
	{
		cStr_SaveToggle_Buffer = NOSAVE_LOGININFO;
	}
	
	//CString cstrBuffer = TEXT("{\"brand_infos\":[{\"brand_id\":\"gongcha316\", \"password\":\"17041\", \"save_toggle\":\"1\"}]}");

#ifdef DEVELOPERS
	cStr_Url = cStr_Url.Mid(7, lstrlen(cStr_Url));
	//MessageBox(cStr_Url);
	return TEXT("{\"brand_infos\":[{\"brand_id\":\"") + cStr_LoginID_Buffer + TEXT("\", \"password\":\"") + cStr_Password_Buffer + TEXT("\", \"save_toggle\":\"") + cStr_SaveToggle_Buffer + TEXT("\", \"url\":\"") + cStr_Url + TEXT("\"}]}");
#else
	return TEXT("{\"brand_infos\":[{\"brand_id\":\"") + cStr_LoginID_Buffer + TEXT("\", \"password\":\"") + cStr_Password_Buffer + TEXT("\", \"save_toggle\":\"") + cStr_SaveToggle_Buffer + TEXT("\"}]}");
#endif // DEVELOPERS

	
}

BOOL CTW_LoginDlg::LoginInfoParsing(CString cstrBuffer)
{
	//CString cstrBuffer = TEXT("{\"brand_infos\":[{\"brand_id\":\"gongcha316\", \"password\":\"17041\", \"save_toggle\":1}]}");

	static char* pbstr;
	
	USES_CONVERSION;
	pbstr = OLE2A(cstrBuffer);

	rapidjson::Document Doc_Json;

	std::string buffer = pbstr;
	// �� �� ���� ������ �޾ƿ� json ������ ���ʿ��� ���ڿ��� �� parsing error �߻���Ű�� ���� ������.
	size_t buffer_size = buffer.rfind("}");
	buffer = buffer.substr(0, buffer_size + 1);
	pbstr = (char*)buffer.c_str();

	Doc_Json.Parse<0>(pbstr);

	const rapidjson::Value& count = Doc_Json["brand_infos"];

	Multi_Count = count.Size();
	
	// �� rapidjson::Value& count ���� JSON Array�� �ƴ� ��� count�� 0�� �ԷµǼ� ���� ������ �� �����Ƿ� ���Ƿ� Multi_Count�� 1�� �־���.
	if (Multi_Count == SINGLE_STRING)
	{
		Multi_Count = 1;
	}
	else
	{
		assert(count.IsArray());
	}

	cStr_LoginID = new CString[Multi_Count];
	cStr_Password = new CString[Multi_Count];
	cStr_SaveToggle = new CString[Multi_Count];

	if (Doc_Json.ParseInsitu<0>(pbstr).HasParseError() == FALSE)
	{
		// ��Ƽī��
		if (count.IsArray())
		{
			for (Card_Count = 0; Card_Count < Multi_Count; Card_Count++)
			{
				cStr_LoginID[Card_Count] = ConvertTextType(d_IE_Version, count[Card_Count]["brand_id"].GetString());
				cStr_Password[Card_Count] = ConvertTextType(d_IE_Version, count[Card_Count]["password"].GetString());
				cStr_SaveToggle[Card_Count] = ConvertTextType(d_IE_Version, count[Card_Count]["save_toggle"].GetString());
				//MessageBox(cStr_LoginID[Card_Count]);
			}

#ifdef DEVELOPERS
			cStr_Url = ConvertTextType(d_IE_Version, Doc_Json["url"].GetString());
			//MessageBox(cStr_Url);
#endif // DEVELOPERS

		}
		else
		{

		}
	}
	else
	{
		// parsing error
	}

	return TRUE;
}

void CTW_LoginDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

	if (cStr_LoginID != NULL)
	{
		delete[] cStr_LoginID;
		cStr_LoginID = NULL;
	}

	if (cStr_Password != NULL)
	{
		delete[] cStr_Password;
		cStr_Password = NULL;
	}

	if (cStr_SaveToggle != NULL)
	{
		delete[] cStr_SaveToggle;
		cStr_SaveToggle = NULL;
	}
}


BOOL CTW_LoginDlg::ReadAndDecrypt(void)
{

	int nKeyLen = strlen((char*)szKey);
	unsigned char chPadding = 254;
	int nPaddingType = 0;

	//CString cstrBuffer = TEXT("{\"brand_infos\":[{\"brand_id\":\"gongcha316\", \"password\":\"17041\", \"save_toggle\":1}]}");			
	CString cstrBuffer = NULL;// = TEXT("{\"brand_infos\":[{\"brand_id\":\"gongcha316\", \"password\":\"17041\", \"save_toggle\":1}]}");			

	unsigned char *szSrc = NULL;

	// ���� �������� �Ǵ°�.
	unsigned char pBuffer[MAX_SIZE] = {0,};
	//char temp[80] = {0, };
	// --zd. ����Ǵ� ������ �α��� �Ϸ� �Ŀ��� ���� ������?
	//if (IsDlgButtonChecked(IDC_BUTTON_CHK_AUTOLOGIN))
	//{
		std::ifstream in("C:\\Touchworks\\Touchworks.DAT");
	////std::ifstream in;
	////in.open(TEXT("Touchworks.DAT"));

		////std::string str;

		//in.get((char*)test, 80);
		in.get((char*)pBuffer, MAX_SIZE);
	//}

	in.close();

	USES_CONVERSION;
	cstrBuffer = A2W((char*)pBuffer);

	int nSrcLen = cstrBuffer.GetLength() + 1;

	unsigned char *pszEncrypted = NULL;
	unsigned char *pszPlain		= NULL;

	unsigned long dwLen = AdjustSourceLen(nSrcLen, SOURCE_MULTIPLE);

	pszEncrypted = (unsigned char*)LocalAlloc(LPTR, dwLen * 2 + 1);

	ByteFillChar(pszEncrypted, chPadding, dwLen * 2);
	//Encrypt(szKey, nKeyLen, chPadding, nPaddingType, pBuffer, dwLen, (char*)pszEncrypted);

	pszPlain = (unsigned char*)LocalAlloc(LPTR, lstrlenA((const char*)pszEncrypted) + 1);
	ByteFillChar(pszPlain, chPadding, lstrlenA((const char*)pszEncrypted));

	

	Decrypt(szKey, nKeyLen, chPadding, nPaddingType, pBuffer, lstrlenA((const char*)pBuffer), (char*)pszPlain);
				
	//USES_CONVERSION;
	CString strTemp;
	strTemp = A2W((char*)pszPlain);

	LoginInfoParsing(strTemp);

	//MessageBox(strTemp);

	LocalFree(pszEncrypted);
	LocalFree(pszPlain);


	return TRUE;
}

