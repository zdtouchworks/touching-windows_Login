
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
	// --zd. 이거 적용하면, 중복 실행에 걸려서, 관리자 모드로 재실행이 안 됨. 중복실행 방지는 관리자 권한에서 어떻게 해야 할 듯?
	// 그리고, 로그인 프로그램만은 실행 안되도록 할까? 업데이트 프로그램에서만 실행 되도록??
	/*
	HANDLE hEvent;
	hEvent = CreateEvent(NULL, FALSE, TRUE, AfxGetAppName());

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(TEXT("프로그램이 이미 실행 중입니다."));
		PostQuitMessage(WM_QUIT);
	}
	*/

	// 응용 프로그램 메니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함호도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Ok)
	{
		MessageBox(0, TEXT("GdiplusStartup Error"), NULL, MB_OK);
	}

	/*
	////// -- zd. 클래스명 지정 /////////////////////////////////////
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

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

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
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리 할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고, 응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}