#pragma once

#include "stdafx.h"

#define UTF8Conv 1

#ifdef UTF8Conv
#include "UTF8Conv.h"
#endif

#define Hangul_String			(1)
#define English_String			(2)
#define Numberic_String			(3)
#define Etc_String				(4)

#ifdef UTF8Conv
CString ConvertTextType(int IE_Version, const char* pText);
#endif

int CheckLanguage(CString pString);
void ConvertThousandsSeparator(CString parm_Data, CString &parm_String);
int GetIEVersion();
void CreateFolder(CString csPath);

#ifdef UTF8Conv

CString ConvertTextType(int IE_Version, const char* pText)
{

	CString cstr_retVal = TEXT("");


	if (IE_Version == 8)
	{
		UTF8_CONVERSION;
		cstr_retVal = UTF82W(pText);
	}
	else
	{
		USES_CONVERSION;
		cstr_retVal = A2W(pText);
	}

	/*
	CString cstr_retVal = TEXT("");

	if (IE_Version >= 9)
	{
		USES_CONVERSION;
		cstr_retVal = A2W(pText);
	}
	else if (IE_Version == 8)
	{
		UTF8_CONVERSION;
		cstr_retVal = UTF82W(pText);
	}
	else
	{
		if (CheckLanguage((CString)pText) == English_String)
		{
			cstr_retVal = pText;
		}
		else
		{

		}
	}
	*/

	return cstr_retVal;
}

#endif

int CheckLanguage(CString pString)
{
	for (int i = 0; i < lstrlen(pString); i++)
	{
		// -- ZD. 한글 - 키보드 외의 ★ 같은 특수문자는 한글로 체크 됨.
		if (0 >= pString.GetAt(i) || 127 < pString.GetAt(i))
		{
			return Hangul_String;
		}
		// -- ZD. 영어 - 아래 두 조건문 다 되지만, 가독성이 좋은 isalpha() 사용
		else if (isalpha(pString.GetAt(i)))
		//else if ((65 <= pString.GetAt(i) && pString.GetAt(i) <= 90) || (97 <= pString.GetAt(i) && pString.GetAt(i) <= 122))
		{
			return English_String;
		}
		// -- ZD. 숫자 - 아래 두 조건문 다 되지만, 가독성이 좋은 isdigit() 사용
		else if (isdigit(pString.GetAt(i)))
		//else if (48 <= pString.GetAt(i) && pString.GetAt(i) <= 57)
		{
			return Numberic_String;
		}
		// -- ZD. 기타 - 추가로 기호도 구현해야 할지도.
		else
		{
			return Etc_String;
		}
	}

	return 0;
}

void ConvertThousandsSeparator(CString parm_Data, CString &parm_String)
{
	// 결과값을 정확하게 구성하기 위해서 ',' 처리 결과가 저장될 문자열 변수를 초기화 한다.
	if (parm_String.GetLength())
	{
		parm_String.Format(TEXT(""));
	}
	
	// 문자열의 길이를 구한다.
	int count = parm_Data.GetLength();

	for (int i = 0; i < count; i++)
	{
		// 3의 배수일 때 ","를 문자열에 추가한다.
		if( i && !(i %3))
		{
			//parm_String = TEXT(",") + parm_String;
			parm_String = TEXT(",") + parm_String;
		}

		// parm_Data 에서 parm_String 으로 숫자 한개씩 순차적으로 복사한다.
		parm_String = CString(parm_Data[count - 1 - i]) + parm_String;
	}
}

int GetIEVersion()
{
	int d_IE_Version = NULL;
	LONG result;
	HKEY hKey;
	DWORD dwType;
	char data[65] = {0, };
	DWORD dataSize = 64;
	wchar_t* ch_result = NULL;
	const size_t cSize = NULL;

	// ---------------------
	// IE의 버전을 얻는다.
	// --------------------
	result = :: RegOpenKeyEx (HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Internet Explorer"), 0, KEY_QUERY_VALUE, &hKey);
	if (result == ERROR_SUCCESS)
	{
		// -- ZD. IE 11? 11이상?에서는 svcVersion 을 체크해야 정확한 버전이 나오지만, 하위 버전에 svcVersion이 없으므로, 그냥 Version 으로 체크. 11에서는 Version 값 9로 나옴.
		// 나중에 Version 이 9일 경우 svcVersion 을 체크하도록 조건문 추가해야 함.
		result = ::RegQueryValueEx(hKey, TEXT("Version"), NULL, &dwType, (unsigned char*)data, &dataSize);
		//result = ::RegQueryValueEx(hKey, TEXT("svcVersion"), NULL, &dwType, (unsigned char*)data, &dataSize);

		CString cstr_IE_Version = NULL;

		cstr_IE_Version.Format(TEXT("%s"), data);

		cstr_IE_Version = cstr_IE_Version.Left(cstr_IE_Version.Find(TEXT(".")));

		if (cstr_IE_Version == TEXT("9"))
		{
			memset(data, 0, sizeof(data));
			cstr_IE_Version = TEXT("");

			result = ::RegQueryValueEx(hKey, TEXT("svcVersion"), NULL, &dwType, (unsigned char*)data, &dataSize);

			cstr_IE_Version.Format(TEXT("%s"), data);

			cstr_IE_Version = cstr_IE_Version.Left(cstr_IE_Version.Find(TEXT(".")));

		}

		//MessageBox(cstr_IE_Version, TEXT(""), MB_OK);

		d_IE_Version = _ttoi(cstr_IE_Version);
	}
	else
	{
		return FALSE;
	}

	RegCloseKey(hKey);

	return d_IE_Version;
}

void CreateFolder(CString csPath)
{
	CString csPrefix(TEXT("")), csToken(TEXT(""));

	int nStart = 0, nEnd;

	while ((nEnd = csPath.Find('/', nStart)) >= 0)
	{
		CString csToken = csPath.Mid(nStart, nEnd - nStart);
		CreateDirectory(csPrefix + csToken, NULL);

		csPrefix += csToken;
		csPrefix += TEXT("/");
		nStart = nEnd + 1;
	}

	csToken = csPath.Mid(nStart);
	CreateDirectory(csPrefix + csToken, NULL);
}