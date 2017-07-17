
// TW_AutoLogin.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTW_AutoLoginApp:
// See TW_AutoLogin.cpp for the implementation of this class
//

class CTW_LoginApp : public CWinAppEx
{
public:
	CTW_LoginApp();
	~CTW_LoginApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	// ---------------- GDI32 용 ----------------
	ULONG_PTR			gpToken; // gdiplus 초기화를 위해 필요함.
	GdiplusStartupInput gpsi; // gdiplus 초기화를 위해 필요함.
	// ------------------------------------------


	DECLARE_MESSAGE_MAP()
};

extern CTW_LoginApp theApp;