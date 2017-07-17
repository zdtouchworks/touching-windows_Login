
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

	CreateFolder(TEXT("C:/Touchworks/Login"));

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
	

	/*
	CWinAppEx::InitInstance();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CTW_LoginDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
	*/
}