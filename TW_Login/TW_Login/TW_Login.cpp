
// TW_Login.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TW_Login.h"
#include "TW_LoginDlg.h"
#include "Elevate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTW_LoginApp

BEGIN_MESSAGE_MAP(CTW_LoginApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTW_LoginApp construction

CTW_LoginApp::CTW_LoginApp()
{
	// support Restart Manager
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	CoInitialize(NULL);

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CTW_LoginApp::~CTW_LoginApp()
{
	/*
	if (gpToken)
	{
		GdiplusShutdown(gpToken);
	}
	*/

	CoUninitialize();
}


// The one and only CTW_LoginApp object

CTW_LoginApp theApp;

// CTW_LoginApp initialization

BOOL CTW_LoginApp::InitInstance()
{
	// --zd. �̰� �����ϸ�, �ߺ� ���࿡ �ɷ���, ������ ���� ������� �� ��. �ߺ����� ������ ������ ���ѿ��� ��� �ؾ� �� ��?
	// �׸���, �α��� ���α׷����� ���� �ȵǵ��� �ұ�? ������Ʈ ���α׷������� ���� �ǵ���??
	/*
	HANDLE hEvent;
	hEvent = CreateEvent(NULL, FALSE, TRUE, AfxGetAppName());

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(TEXT("���α׷��� �̹� ���� ���Դϴ�."));
		PostQuitMessage(WM_QUIT);
	}
	*/

	// ���� ���α׷� �޴��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ ����ȣ����
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Ok)
	{
		MessageBox(0, TEXT("GdiplusStartup Error"), NULL, MB_OK);
	}

	/*
	////// -- zd. Ŭ������ ���� /////////////////////////////////////
	WNDCLASS wc;
	// Get the info for this class.
	// #32770 is the default class name for dialogs boxes.
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);

	CString aa;
	aa.LoadStringW(AFX_IDS_APP_ID);

	// Change the name of the class.
	wc.lpszClassName = _T("Touching_Login");
	AfxRegisterClass(&wc);
	////////////////////////////////////////////////////////////////
	*/

	WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	::GetClassInfoExW(0, WC_DIALOG, &wc);
	wc.lpszClassName = TEXT("Touching Login");
	wc.style &- ~CS_DBLCLKS;
	::RegisterClassExW(&wc);

	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	CElevate pElevate;

	if (pElevate.GetNeedElevate() == 3)
	{
		pElevate.SelfExecuteWithElevation();

		PostQuitMessage(WM_QUIT);
	}

	CreateFolder(TEXT("C:\\Touchworks\\Login"));

	//SetRegistryKey(TEXT("Touchworks_Login"));

	CTW_LoginDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó�� ��
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�, ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}