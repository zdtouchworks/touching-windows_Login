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

// �α��� �Լ�, ���� �α����� ��ư�� Ŭ���� ��ó�� ���� �ڵ��α��� �ǵ��� �Ѵ�.
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

// �α��� �Լ�, ���� �α����� �ڹٽ�ũ��Ʈ�� ���� ��Ŵ���ν� �ڵ��α��� �ǵ��� �Ѵ�.
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

// ���ο� �� �������� ���� ��Ų��.
BOOL CTW_LoginWnd::CreateNewBrowser()
{
	if (m_pWebBrowser2) // �� �� ������ �����͸� ���� �ִٸ� �����. (���̻� ��Ʈ�� �� �� ����.)
	{
		UnAdviseBrowser();
		m_pWebBrowser2->Release();
		m_pWebBrowser2 = NULL;
	}

	// �� ���ͳ� �ͽ��÷η��� ���� ��Ų��.
	HRESULT hr = CoCreateInstance (CLSID_InternetExplorer, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser2);

	if (SUCCEEDED(hr) && m_pWebBrowser2)
	{
		hr = m_pWebBrowser2->put_StatusBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_ToolBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_MenuBar(VARIANT_TRUE);
		hr = m_pWebBrowser2->put_Visible(VARIANT_TRUE);

		AdviseBrowser(); // �̺�Ʈ ��ũ
		NavigateWeb(); // �� Ž��
	}
	else
		return FALSE;

	return TRUE;
}

// �̺�Ʈ ��ũ
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

// �̺�Ʈ ��ũ ����
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

// �� Ž���� �Ѵ�.
void CTW_LoginWnd::NavigateWeb()
{
	COleVariant vUrl(cstr_Url, VT_BSTR), vFlags(long(navNoHistory | navNoReadFromCache | navNoWriteToCache), VT_I4), vNull(LPCTSTR(NULL), VT_BSTR);

	m_pWebBrowser2->Navigate2(&vUrl, &vFlags, &vNull, &vNull, &vNull);

	HWND hWnd_Parent = ::FindWindow(TEXT("Touching Login"), TEXT("�α���"));
	::SetWindowPos(hWnd_Parent, wndBottom, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	
	vUrl.Clear();
	vFlags.Clear();
	vNull.Clear();
}

// �����Ų ���� ����Ǹ� �߻��ϴ� �̺�Ʈ
void CTW_LoginWnd::OnQuit()
{
	// ����
	if (m_pWebBrowser2)
	{
		UnAdviseBrowser();
		m_pWebBrowser2->Release();
		m_pWebBrowser2 = NULL;
	}
}

// �� ������ �ε��� �Ϸ�Ǹ�
void CTW_LoginWnd::OnDocumentComplete(IDispatch *pDisp,VARIANT *URL)
{
	LPDISPATCH pThisDisp = NULL;

	HRESULT hr = m_pWebBrowser2->get_Application(&pThisDisp);
	if(FAILED(hr) || !pThisDisp)
		return;

	if(pDisp == pThisDisp)//���������� ���������� �������� �������� ����Ͽ� ������ �ε��� �Ϸ�Ǹ�
	{

		Sleep(1000);
		UnAdviseBrowser();//�̺�Ʈ�� ���� �ʵ��� �Ѵ�

		if(!PutText(cstr_FormName , cstr_IDObjectID , cstr_ID))//���̵� �Է��Ѵ�
			return;

		if(!PutText(cstr_FormName , cstr_PwdObjectID , cstr_Pwd))//��ȣ�� �Է��Ѵ�
			return;

		if(bScriptLogin)
			LoginByScript();//�ڹٽ�ũ��Ʈ�� �ڵ��α��� ��Ų��
		else;
			LoginByButton();//��ư�� Ŭ���� ȿ���� �̿��Ͽ� �ڵ��α��� ��Ų��
	}
	
	pThisDisp->Release();
}

/******************************************************************************************************************************
	<form name="loginform" id="loginform" method="post" action="https://logins.daum.net/Mail-bin/login.cgi">
	<input type="text" name="name_id" id="id" maxlength="15" value="" title="���̵��Է�" tabindex="2" />
	<input type="password" name="name_enpw" id="enpw" maxlength="32" value="" title="��й�ȣ�Է�" tabindex="3" />
	</form>

	 �̶�� �ҽ��� ���� ��
	 ���̵� �Է��ϰ� �ʹٸ�, cstr_FormName = loginform, cstr_ObjectID�� id, cstr_PutText�� ����� ���̵�
	 ��ȣ�� �Է��ϰ� �ʹٸ�, cstr_FormName = loginform, cstr_ObjectID�� enpw, cstr_PutText�� ����� ��ȣ
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
		//������ ����� ���Ѵ�
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;
		ULONG uFetched = 0;

		//�����Ӹ���� ��ȸ
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
	pInputElem->put_value( bstrPutText );//�ؽ�Ʈ �Է�
		
	return TRUE;
}

// ��ư�� Ŭ���� ȿ���� ���� �ڵ� �α��� ��Ų��.
// �� �� ��ư�� ���� ��ư �Ӹ� �ƴ϶�, image ������ ��ư�� �ش�ȴ�.
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
		if (FAILED(hr) || !pContainer)//4.64���� ����
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//������ ����� ���Ѵ�
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;//CComPtr<IUnknown> pUnk���� �����ϴ� ��� ���� �߻�...������غ��� �������� ��� ��������� �϶�� ���� ����..�Ƹ� ����� �޸� ���� ���� �ݺ����� ���Ǵٺ��� �׷��� �ƴѰ� ����...
		ULONG uFetched = 0;

		//�����Ӹ���� ��ȸ�ϸ鼭 ȭ��ũ�⸦ ���ϰ� �� �� ���� ū ����  nFrameWidth, nFrameHeight�� �����Ѵ�
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

// ��ũ��Ʈ�� �̿��Ͽ� �ڵ����� ��Ų��.
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
		//������ ����� ���Ѵ�
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;//CComPtr<IUnknown> pUnk���� �����ϴ� ��� ���� �߻�...������غ��� �������� ��� ��������� �϶�� ���� ����..�Ƹ� ����� �޸� ���� ���� �ݺ����� ���Ǵٺ��� �׷��� �ƴѰ� ����...
		ULONG uFetched = 0;

		//�����Ӹ���� ��ȸ�ϸ鼭 ȭ��ũ�⸦ ���ϰ� �� �� ���� ū ����  nFrameWidth, nFrameHeight�� �����Ѵ�
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
	//JavaScript ����
	hr = pScript->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD,&dispparams, &vaResult, &excepInfo, &nArgErr);
	delete [] dispparams.rgvarg;
	pScript->Release();

	if(FAILED(hr))
		return FALSE;
	
	return TRUE;
}
