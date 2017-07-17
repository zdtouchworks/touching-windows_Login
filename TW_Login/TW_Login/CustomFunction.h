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
		// -- ZD. �ѱ� - Ű���� ���� �� ���� Ư�����ڴ� �ѱ۷� üũ ��.
		if (0 >= pString.GetAt(i) || 127 < pString.GetAt(i))
		{
			return Hangul_String;
		}
		// -- ZD. ���� - �Ʒ� �� ���ǹ� �� ������, �������� ���� isalpha() ���
		else if (isalpha(pString.GetAt(i)))
		//else if ((65 <= pString.GetAt(i) && pString.GetAt(i) <= 90) || (97 <= pString.GetAt(i) && pString.GetAt(i) <= 122))
		{
			return English_String;
		}
		// -- ZD. ���� - �Ʒ� �� ���ǹ� �� ������, �������� ���� isdigit() ���
		else if (isdigit(pString.GetAt(i)))
		//else if (48 <= pString.GetAt(i) && pString.GetAt(i) <= 57)
		{
			return Numberic_String;
		}
		// -- ZD. ��Ÿ - �߰��� ��ȣ�� �����ؾ� ������.
		else
		{
			return Etc_String;
		}
	}

	return 0;
}

void ConvertThousandsSeparator(CString parm_Data, CString &parm_String)
{
	// ������� ��Ȯ�ϰ� �����ϱ� ���ؼ� ',' ó�� ����� ����� ���ڿ� ������ �ʱ�ȭ �Ѵ�.
	if (parm_String.GetLength())
	{
		parm_String.Format(TEXT(""));
	}
	
	// ���ڿ��� ���̸� ���Ѵ�.
	int count = parm_Data.GetLength();

	for (int i = 0; i < count; i++)
	{
		// 3�� ����� �� ","�� ���ڿ��� �߰��Ѵ�.
		if( i && !(i %3))
		{
			//parm_String = TEXT(",") + parm_String;
			parm_String = TEXT(",") + parm_String;
		}

		// parm_Data ���� parm_String ���� ���� �Ѱ��� ���������� �����Ѵ�.
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
	// IE�� ������ ��´�.
	// --------------------
	result = :: RegOpenKeyEx (HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Internet Explorer"), 0, KEY_QUERY_VALUE, &hKey);
	if (result == ERROR_SUCCESS)
	{
		// -- ZD. IE 11? 11�̻�?������ svcVersion �� üũ�ؾ� ��Ȯ�� ������ ��������, ���� ������ svcVersion�� �����Ƿ�, �׳� Version ���� üũ. 11������ Version �� 9�� ����.
		// ���߿� Version �� 9�� ��� svcVersion �� üũ�ϵ��� ���ǹ� �߰��ؾ� ��.
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