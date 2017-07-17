// CTW_LoginWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TW_Login.h"
#include "TW_LoginWnd.h"

#include <ExDisp.h>
#include <ExDispid.h>
#include <MsHTML.h>
#include <MsHtmdid.h>
#include <ShObjIdl.h>
#include <ShlObj.h>
#include <afxctl.h>

// CTW_LoginWnd

	IMPLEMENT_DYNAMIC(CTW_LoginWnd, CWnd)

	BEGIN_DISPATCH_MAP(CTW_LoginWnd, CWnd)
		DISP_FUNCTION_ID(CTW_LoginWnd, "OnQuit", DISPID_ONQUIT, OnQuit, VT_EMPTY, VTS_NONE)
		DISP_FUNCTION_ID(CTW_LoginWnd, "OnDocumentComplete", DISPID_DOCUMENTCOMPLETE, OnDocumentComplete, VT_EMPTY, VTS_DISPATCH VTS_PVARIANT)
	END_DISPATCH_MAP()

CTW_LoginWnd::CTW_LoginWnd()
{
	EnableAutomation();

	m_pWebBrowser2 = NULL;
	bScriptLogin = TRUE;
	m_dwCookie = 0;
}

CTW_LoginWnd::~CTW_LoginWnd()
{
	if (m_pWebBrowser2)
	{
		UnAdviseBrowser();
		m_pWebBrowser2->Release();
		m_pWebBrowser2 = NULL;
	}
}

BEGIN_MESSAGE_MAP(CTW_LoginWnd, CWnd)
END_MESSAGE_MAP()

// CTW_LoginWnd message handlers

// 로그인 함수, 최종 로그인은 버튼을 클릭한 것처럼 만들어서 자동로그인 되도록 한다.
BOOL CTW_LoginWnd::AutoLoginByButton(CString strUrl, CString strID, CString strPwd, CString strFormName, CString strIDObjectID, CString strPwdObjectID, CString strClickObjectID)
{
	this->cstr_Url = strUrl;
	this->cstr_ID = strID;
	this->cstr_Pwd = strPwd;
	this->cstr_FormName = strFormName;
	this->cstr_IDObjectID = strIDObjectID;
	this->cstr_PwdObjectID = strPwdObjectID;
	this->cstr_ClickObjectID = strClickObjectID;

	if (!CreateNewBrowser())
		return FALSE;

	bScriptLogin = FALSE;

	return TRUE;
}

// 로그인 함수, 최종 로그인은 자바스크립트를 실행 시킴으로써 자동로그인 되도록 한다.
BOOL CTW_LoginWnd::AutoLoginByScript(CString strUrl, CString strID, CString strPwd, CString strFormName, CString strIDObjectID, CString strPwdObjectID, CString strClickObjectID)
{
	this->cstr_Url = strUrl;
	this->cstr_ID = strID;
	this->cstr_Pwd = strPwd;
	this->cstr_FormName = strFormName;
	this->cstr_IDObjectID = strIDObjectID;
	this->cstr_PwdObjectID = strPwdObjectID;
	this->cstr_ClickObjectID = strClickObjectID;

	if (!CreateNewBrowser())
		return FALSE;

	bScriptLogin = TRUE;

	return TRUE;
}

// 새로운 웹 브라우저를 실행 시킨다.
BOOL CTW_LoginWnd::CreateNewBrowser()
{
	if (m_pWebBrowser2) // 기 웹 브라우저 포인터를 갖고 있다면 지운다. (더이상 컨트롤 할 수 없다.)
	{
		UnAdviseBrowser();
		m_pWebBrowser2->Release();
		m_pWebBrowser2 = NULL;
	}

	// 새 인터넷 익스플로러를 실행 시킨다.
	HRESULT hr = CoCreateInstance (CLSID_InternetExplorer, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser2);

	if (SUCCEEDED(hr) && m_pWebBrowser2)
	{
		hr = m_pWebBrowser2->put_StatusBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_ToolBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_MenuBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_Visible(VARIANT_TRUE);

		AdviseBrowser(); // 이벤트 싱크
		NavigateWeb(); // 웹 탐색
	}
	else
		return FALSE;

	return TRUE;
}

// 이벤트 싱크
BOOL CTW_LoginWnd::AdviseBrowser()
{
	if (m_pWebBrowser2)
	{
		LPUNKNOWN pUnSink = GetIDispatch(FALSE);
		AfxConnectionAdvise((LPUNKNOWN)m_pWebBrowser2, DIID_DWebBrowserEvents2, pUnSink, FALSE, &m_dwCookie);
		pUnSink->Release();
		return TRUE;
	}
	else
		return FALSE;
}

// 이벤트 싱크 해제
BOOL CTW_LoginWnd::UnAdviseBrowser()
{
	if (m_dwCookie != 0 && m_pWebBrowser2 != NULL)
	{
		LPUNKNOWN pUnSink = GetIDispatch(FALSE);
		AfxConnectionUnadvise((LPUNKNOWN)m_pWebBrowser2, DIID_DWebBrowserEvents2, pUnSink, FALSE, m_dwCookie);
		pUnSink->Release();
		m_dwCookie = 0;

		return TRUE;
	}
	else
		return FALSE;
}

// 웹 탐색을 한다.
void CTW_LoginWnd::NavigateWeb()
{
	COleVariant vUrl(cstr_Url, VT_BSTR), vFlags(long(navNoHistory | navNoReadFromCache | navNoWriteToCache), VT_I4), vNull(LPCTSTR(NULL), VT_BSTR);

	m_pWebBrowser2->Navigate2(&vUrl, &vFlags, &vNull, &vNull, &vNull);

	HWND hWnd_Parent = ::FindWindow(TEXT("Touching Login"), TEXT("로그인"));
	::SetWindowPos(hWnd_Parent, wndBottom, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	
	vUrl.Clear();
	vFlags.Clear();
	vNull.Clear();
}

// 실행시킨 웹이 종료되면 발생하는 이벤트
void CTW_LoginWnd::OnQuit()
{
	// 해제
	if (m_pWebBrowser2)
	{
		UnAdviseBrowser();
		m_pWebBrowser2->Release();
		m_pWebBrowser2 = NULL;
	}
}

// 웹 페이지 로딩이 완료되면
void CTW_LoginWnd::OnDocumentComplete(IDispatch *pDisp,VARIANT *URL)
{
	LPDISPATCH pThisDisp = NULL;

	HRESULT hr = m_pWebBrowser2->get_Application(&pThisDisp);
	if(FAILED(hr) || !pThisDisp)
		return;

	if(pDisp == pThisDisp)//웹페이지가 프레임으로 나누어져 있을때를 대비하여 마지막 로딩이 완료되면
	{

		Sleep(1000);
		UnAdviseBrowser();//이벤트는 받지 않도록 한다

		if(!PutText(cstr_FormName , cstr_IDObjectID , cstr_ID))//아이디를 입력한다
			return;

		if(!PutText(cstr_FormName , cstr_PwdObjectID , cstr_Pwd))//암호를 입력한다
			return;

		if(bScriptLogin)
			LoginByScript();//자바스크립트로 자동로그인 시킨다
		else;
			LoginByButton();//버튼을 클릭한 효과를 이용하여 자동로그인 시킨다
	}
	
	pThisDisp->Release();
}

/******************************************************************************************************************************
	<form name="loginform" id="loginform" method="post" action="https://logins.daum.net/Mail-bin/login.cgi">
	<input type="text" name="name_id" id="id" maxlength="15" value="" title="아이디입력" tabindex="2" />
	<input type="password" name="name_enpw" id="enpw" maxlength="32" value="" title="비밀번호입력" tabindex="3" />
	</form>

	 이라는 소스가 있을 때
	 아이디를 입력하고 싶다면, cstr_FormName = loginform, cstr_ObjectID는 id, cstr_PutText는 사용자 아이디
	 암호를 입력하고 싶다면, cstr_FormName = loginform, cstr_ObjectID는 enpw, cstr_PutText는 사용자 암호
*******************************************************************************************************************************/

BOOL CTW_LoginWnd::PutText(CString strFormName, CString strObjectID, CString strPutText)
{
	CComPtr<IDispatch> pDisp = NULL;
	CComPtr<IDispatch> pActiveDisp = NULL;
	CComPtr<IHTMLDocument2> pDispDoc2 = NULL;
	CComPtr<IHTMLFormElement> pForm = NULL;
	HRESULT hr;
	
	hr = m_pWebBrowser2->get_Document(&pDisp);
	if(FAILED(hr) || !pDisp)
		return FALSE;

	hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDispDoc2);
	
	long nForms = 0;
	CComPtr<IHTMLElementCollection> pFormCollection=NULL;
	hr = pDispDoc2->get_forms(&pFormCollection);
	if(FAILED(hr) || !pFormCollection)
		return FALSE;

	hr = pFormCollection->get_length(&nForms);
	if(FAILED(hr))
		return FALSE;

	CComPtr<IDispatch> pDispForm = NULL;

	hr = pFormCollection->item(COleVariant(strFormName.AllocSysString(), VT_BSTR), COleVariant((long)0), &pDispForm);
	
	
	if(FAILED(hr) || !pDispForm)
	{
		BOOL bSuccess = FALSE;
		CComPtr<IOleContainer> pContainer = NULL;

		//Get the container
		hr = pDisp->QueryInterface(IID_IOleContainer, (void**)&pContainer);
		if (FAILED(hr) || !pContainer)
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//프레임 목록을 구한다
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;
		ULONG uFetched = 0;

		//프레임목록을 순회
		for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
		{
			CComPtr<IWebBrowser2> pBrowser = NULL;

			hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pBrowser);

			if (SUCCEEDED(hr) && pBrowser)
			{

				CComPtr<IDispatch> pFrameDisp = NULL;
				CComPtr<IHTMLDocument2> pFrameDoc2 = NULL;
				CComPtr<IHTMLElementCollection> pFrameFormCollection=NULL;
				CComPtr<IDispatch> pFrameDispForm = NULL;

				hr = pBrowser->get_Document(&pFrameDisp);
				if(FAILED(hr) && !pFrameDisp)
					continue;
				hr = pFrameDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pFrameDoc2);
				if(FAILED(hr) || !pFrameDoc2)
					continue;

				hr = pFrameDoc2->get_forms(&pFrameFormCollection);
				if(FAILED(hr) || !pFrameFormCollection)
					continue;

				hr = pFrameFormCollection->get_length(&nForms);
				if(FAILED(hr))
					continue;

				hr = pFrameFormCollection->item(COleVariant(strFormName.AllocSysString(), VT_BSTR), COleVariant((long)0), &pFrameDispForm);
				if(FAILED(hr) || !pFrameDispForm)
					continue;

				hr = pFrameDispForm->QueryInterface(IID_IHTMLFormElement, (void**)&pForm);
				if(FAILED(hr) || !pForm)
					continue;

				bSuccess = TRUE;
				break;
			}

			pUnk->Release();
			pUnk = NULL;
		}

		if(!bSuccess)
			return FALSE;
	}
	else
	{
		hr = pDispForm->QueryInterface(IID_IHTMLFormElement, (void**)&pForm);
		if(FAILED(hr) || !pForm)
			return FALSE;
	}

	if(!pForm)
		return FALSE;

	CComPtr<IHTMLInputTextElement> pInputElem = NULL;
	CComPtr<IDispatch> pDispInputForm = NULL;

	hr = pForm->item(COleVariant(strObjectID.AllocSysString()),COleVariant(long(0)),&pDispInputForm);
	if(FAILED(hr) || !pDispInputForm)
		return FALSE;
	
	hr = pDispInputForm->QueryInterface(IID_IHTMLInputTextElement, (void**)&pInputElem);
	if(FAILED(hr) || !pInputElem)
		return FALSE;

	CComBSTR bstrPutText(strPutText);
	pInputElem->select();
	pInputElem->put_value( bstrPutText );//텍스트 입력
		
	return TRUE;
}

// 버튼을 클릭한 효과를 내어 자동 로그인 시킨다.
// 이 때 버튼은 실제 버튼 뿐만 아니라, image 형태의 버튼도 해당된다.
BOOL CTW_LoginWnd::LoginByButton()
{
	CComPtr<IHTMLElement> pElement = NULL;
	CComPtr<IDispatch> pDisp = NULL;
	CComPtr<IHTMLDocument3> pDoc3 = NULL;
	BOOL bSuccess = FALSE;

	HRESULT hr;
	
	hr = m_pWebBrowser2->get_Document(&pDisp);

	if(FAILED(hr) || !pDisp)
		return FALSE;

	
	hr = pDisp->QueryInterface(IID_IHTMLDocument3, (void **)&pDoc3);
	if(FAILED(hr) || !pDoc3)
		return FALSE;

	CComBSTR bstrClickObjectID(cstr_ClickObjectID);
	
	hr = pDoc3->getElementById(bstrClickObjectID, &pElement);
	if(SUCCEEDED(hr) && pElement)
	{
		pElement->click();
		bSuccess = TRUE;
	}
	else
	{
		CComPtr<IOleContainer> pContainer = NULL;

		//Get the container
		hr = pDisp->QueryInterface(IID_IOleContainer, (void**)&pContainer);
		if (FAILED(hr) || !pContainer)//4.64버전 수정
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//프레임 목록을 구한다
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;//CComPtr<IUnknown> pUnk으로 선언하는 경우 에러 발생...디버깅해보니 에러나는 경우 명시적으로 하라고 나와 있음..아마 명시적 메모리 해제 없이 반복문에 사용되다보니 그런게 아닌가 싶음...
		ULONG uFetched = 0;

		//프레임목록을 순회하면서 화면크기를 구하고 그 중 가장 큰 값을  nFrameWidth, nFrameHeight에 저장한다
		for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
		{
			CComPtr<IWebBrowser2> pBrowser = NULL;

			hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pBrowser);

			if (SUCCEEDED(hr) && pBrowser)
			{

				CComPtr<IDispatch> pFrameDisp = NULL;
				CComPtr<IHTMLDocument3> pFrameDoc = NULL;
				CComPtr<IHTMLElement> pFrameElem = NULL;

				hr = pBrowser->get_Document(&pFrameDisp);
				if(FAILED(hr) && !pFrameDisp)
					continue;
				hr = pFrameDisp->QueryInterface(IID_IHTMLDocument3, (void**)&pFrameDoc);
				if(FAILED(hr) || !pFrameDoc)
					continue;

				hr = pFrameDoc->getElementById(bstrClickObjectID, &pFrameElem);
				if(FAILED(hr) || !pFrameElem)
					continue;

				pFrameElem->click();
				bSuccess = TRUE;
				break;
			}
			
			pUnk->Release();
			pUnk = NULL;
		}
	}

	return bSuccess;
}

// 스크립트를 이용하여 자동실행 시킨다.
BOOL CTW_LoginWnd::LoginByScript()
{
	CComPtr<IHTMLElement> pElement = NULL;
	CComPtr<IDispatch> pDisp = NULL;
	DISPID dispid = NULL;
	CComPtr<IHTMLDocument2> pDoc2 = NULL;
	LPDISPATCH pScript = NULL;
	CComBSTR bstrScriptName(cstr_ScriptName);
	CStringArray paramArray;


	BOOL bSuccess = FALSE;

	HRESULT hr;

	hr = m_pWebBrowser2->get_Document(&pDisp);

	if(FAILED(hr) || !pDisp)
		return FALSE;


	hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void **)&pDoc2);
	if(FAILED(hr) || !pDoc2)
		return FALSE;

	hr = pDoc2->get_Script(&pScript);
	if(FAILED(hr) || !pScript)
		return FALSE;

	hr = pScript->GetIDsOfNames(IID_NULL, &bstrScriptName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);

	if(FAILED(hr) || !pScript)
	{
		if(pScript)
		{
			pScript->Release();
			pScript = NULL;
		}
		
		CComPtr<IOleContainer> pContainer = NULL;

		//Get the container
		hr = pDisp->QueryInterface(IID_IOleContainer, (void**)&pContainer);
		if (FAILED(hr) || !pContainer)
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//프레임 목록을 구한다
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;//CComPtr<IUnknown> pUnk으로 선언하는 경우 에러 발생...디버깅해보니 에러나는 경우 명시적으로 하라고 나와 있음..아마 명시적 메모리 해제 없이 반복문에 사용되다보니 그런게 아닌가 싶음...
		ULONG uFetched = 0;

		//프레임목록을 순회하면서 화면크기를 구하고 그 중 가장 큰 값을  nFrameWidth, nFrameHeight에 저장한다
		for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
		{
			CComPtr<IWebBrowser2> pBrowser = NULL;

			hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pBrowser);

			if (SUCCEEDED(hr) && pBrowser)
			{

				CComPtr<IDispatch> pFrameDisp = NULL;
				CComPtr<IHTMLDocument2> pFrameDoc2 = NULL;
				CComPtr<IHTMLElement> pFrameElem = NULL;

				hr = pBrowser->get_Document(&pFrameDisp);
				if(FAILED(hr) && !pFrameDisp)
					continue;
				hr = pFrameDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pFrameDoc2);
				if(FAILED(hr) || !pFrameDoc2)
					continue;
				
				hr = pFrameDoc2->get_Script(&pScript);
				if(FAILED(hr) || !pScript)
					continue;
				
				hr = pScript->GetIDsOfNames(IID_NULL, &bstrScriptName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr) || !pDisp)
				{
					pScript->Release();
					pScript = NULL;
					continue;
				}

				bSuccess = TRUE;
				break;
			}

			pUnk->Release();
			pUnk = NULL;
		}

	}
	
	if(!pScript || !dispid)
		return FALSE;
	
	const int arraySize = paramArray.GetSize();

	//Putting parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs      = arraySize;
	dispparams.rgvarg     = new VARIANT[dispparams.cArgs];

	for( int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}

	dispparams.cNamedArgs = 0;


	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1; // initialize to invalid arg
	//JavaScript 실행
	hr = pScript->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD,&dispparams, &vaResult, &excepInfo, &nArgErr);
	delete [] dispparams.rgvarg;
	pScript->Release();

	if(FAILED(hr))
		return FALSE;
	
	return TRUE;
}
