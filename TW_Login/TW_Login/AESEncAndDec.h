#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <WinCrypt.h> // visual studio 2008 (v90) 일 때 include 필요. 2012일 땐 이미 include 되어 있음.
#define AES_KEY_LENGTH		32
#define SOURCE_MULTIPLE		16
#define BLOCK_SIZE			16

#pragma comment(lib, "Advapi32.lib")
#pragma warning(disable:4996)

void HandleError(char *s);

int ByteReplace(unsigned char* pszSource, int iLength, char chOld, char chNew, int iStart, int iEnd);

void ByteFillChar(unsigned char* pszByte, char ch, int iLength);

int ByteCopyN(unsigned char* pszDest, const unsigned char* pszSource, int iLength);

void HexToHexString(unsigned long nHexLen, void* pv, char* sz);

unsigned long HexStringToHex(unsigned long nStrLen, const unsigned char* sz, PBYTE szHex);

int AdjustSourceLen(int nSrcLen, int nMultiple);

int AES_Initialize(const unsigned char* lpszKey, const int nKeyLen, const unsigned char chPadding, int nPaddingType, HCRYPTPROV* hProv, HCRYPTKEY* hKey);

BOOL AES_Encrypt(HCRYPTKEY* phKey, unsigned const char* lpszSource, int nLen, unsigned char* pszEncrypted);

BOOL AES_Decrypt(HCRYPTKEY* phKey, unsigned const char* pszEncryptedSrc, int nLen, unsigned char* pszDecrypted);

char* Encrypt(const unsigned char* lpszKey, unsigned long nKeyLen, const unsigned char chPadding, int nPaddingType, const unsigned char* lpszPlain, unsigned long nPlainLen, char* pszEncryptedHexStr);

char* Decrypt(const unsigned char* lpszKey, unsigned long nKeyLen, const unsigned char chPadding, int nPaddingType, const unsigned char* lpszEncHex, unsigned long nEncHexLen, char* pszPlain);

void HandleError(char *sz) 
{ 
	printf("An error occurred in running the program.\n"); 
	printf("%s\n",sz); 
} 

int AdjustSourceLen(int nSrcLen, int nMultiple)
{
	int nAdjustLen = 0;
	int nRemainder = 0;

	//adjust size, 16 Multiple
	nRemainder = nSrcLen % nMultiple;
	nAdjustLen = nRemainder ? nMultiple - nRemainder : 0;

	return (nSrcLen + nAdjustLen);
}

void ByteFillChar(unsigned char* pszByte, char ch, int iLength)
{
	int i = 0;

	for (i = 0; i < iLength; i++)
	{
		*pszByte++ = ch;
	}
	return;
}

int ByteCopyN(unsigned char* pszDest, const unsigned char* pszSource, int iLength)
{
	int i = 0;
	
	for(i = 0;i < iLength; i++)
	{
		*(pszDest + i) = *(pszSource + i);
	}
	
	return i;
}

int ByteReplace(unsigned char* pszSource, int iLength, char chOld, char chNew, int iStart, int iEnd)
{
	int idx		= 0;
	int iLast   = 0;
	int iRepl   = 0;

	iLast = (iEnd == -1) ? iLength : iEnd;

	for (idx = iStart; idx < iLast; idx++)
	{
		if (*(pszSource + idx) == chOld) {
			*(pszSource + idx) = chNew;
			++iRepl;
		}
	}
	return iRepl;
}


void HexToHexString(unsigned long nHexLen, void* pv, char* sz)
{
	BYTE* pb = (BYTE*) pv; 
	unsigned long i = 0;               
	__int16 b;
	
	for (i = 0; i < nHexLen; i++)
	{
		b = (*pb & 0xF0) >> 4;
		*sz++ = (b <= 9) ? (char)(b + '0') : (char)((b - 10) + 'A');
		b = *pb & 0x0F;
		*sz++ = (b <= 9) ? (char)(b + '0') : (char)((b - 10) + 'A');
		pb++;
	}
	//*sz++ = 0;
}

unsigned long HexStringToHex(unsigned long nStrLen, const unsigned char* sz, PBYTE szHex)
{	
	unsigned long iCnt = 0;
	int iRes = 0;
	char ch = 0;
	
	// lower ==> upper
	int iUp = 0;
	
	for (iCnt = 0; iCnt < nStrLen; iCnt +=2)
	{
		iUp = (*sz>='a') && (*sz<='z') ? 10  - 'a': 10 - 'A';
		ch = (*sz > '9') ? (char)(*sz + iUp) : (char)(*sz - '0');
		iRes = (ch << 4);
		sz++;
		
		iUp = (*sz>='a') && (*sz<='z') ? 10  - 'a': 10 - 'A';
		ch = (*sz > '9') ? (char)(*sz + iUp) : (char)(*sz - '0');
		iRes += ch;
		sz++;
		
		*(szHex+ iCnt/2) = (BYTE)iRes;
	}

	return (iCnt/2); 
}

char* Encrypt(const unsigned char* lpszKey, unsigned long nKeyLen, 
			  const unsigned char chPadding, int nPaddingType,
			  const unsigned char* lpszPlain, unsigned long nPlainLen,
			  char* pszEncryptedHexStr)
{
	HCRYPTPROV hProv = 0;
	HCRYPTKEY  hKey  = 0;
	HCRYPTKEY  hDuplicateKey = 0;

	unsigned char *pszBuf  = NULL;
	unsigned long dwAdjust = 0;
	unsigned long i = 0;

	*pszEncryptedHexStr = 0x00;

	if (!AES_Initialize(lpszKey, nKeyLen, chPadding, nPaddingType, &hProv, &hKey))
	{
		if (pszBuf != NULL)
		LocalFree(pszBuf);

		if (hKey != 0)	
			CryptDestroyKey(hKey);

		if (hProv != 0)
			CryptReleaseContext(hProv,0);

		return pszEncryptedHexStr;
	}

	dwAdjust = AdjustSourceLen(nPlainLen, SOURCE_MULTIPLE);
	pszBuf = (unsigned char*)LocalAlloc(LPTR, dwAdjust+1);
	
	CryptDuplicateKey(hKey, NULL, 0, &hDuplicateKey);
	for (i = 0; i < dwAdjust; i += BLOCK_SIZE) 
		AES_Encrypt(&hDuplicateKey, (const unsigned char*)(lpszPlain+i), 
					BLOCK_SIZE, (unsigned char*)(pszBuf+i));
	CryptDestroyKey(hDuplicateKey);

	HexToHexString(dwAdjust, pszBuf, pszEncryptedHexStr);

	if (pszBuf != NULL)
		LocalFree(pszBuf);

	if (hKey != 0)	
		CryptDestroyKey(hKey);

	if (hProv != 0)
		CryptReleaseContext(hProv,0);

	return pszEncryptedHexStr;
}

char* Decrypt(const unsigned char* lpszKey, unsigned long nKeyLen,
			  const unsigned char chPadding, int nPaddingType,
			  const unsigned char* lpszEncHex, unsigned long nEncHexLen,
			  char* pszPlain)
{
	HCRYPTPROV hProv = 0;
	HCRYPTKEY  hKey  = 0;
	HCRYPTKEY  hDuplicateKey = 0;

	unsigned char *pszBuf  = NULL;
	unsigned long dwAdjust = 0;
	unsigned long i = 0;

	*pszPlain = 0x00;

	if (!AES_Initialize(lpszKey, nKeyLen, chPadding, nPaddingType, &hProv, &hKey))
	{
		if (pszBuf != NULL)
		LocalFree(pszBuf);

		if (hKey != 0)	
			CryptDestroyKey(hKey);

		if (hProv != 0)
			CryptReleaseContext(hProv,0);

		return pszPlain;
	}

	dwAdjust = nEncHexLen;
	pszBuf = (unsigned char*)LocalAlloc(LPTR, dwAdjust+1);
	
	HexStringToHex(dwAdjust, lpszEncHex, pszBuf);

	CryptDuplicateKey(hKey, NULL, 0, &hDuplicateKey);
	for (i = 0; i < dwAdjust; i += BLOCK_SIZE) 
		AES_Decrypt(&hDuplicateKey, (const unsigned char*)(pszBuf+i), 
					BLOCK_SIZE, (unsigned char*)(pszPlain+i));
	CryptDestroyKey(hDuplicateKey);

	if (pszBuf != NULL)
		LocalFree(pszBuf);

	if (hKey != 0)	
		CryptDestroyKey(hKey);

	if (hProv != 0)
		CryptReleaseContext(hProv,0);

	return pszPlain;
}

BOOL AES_Decrypt(HCRYPTKEY* phKey, unsigned const char* pszEncryptedSrc, 
				 int nSrcLen, unsigned char* pszDecrypted)
{
	BOOL bRetVal = TRUE;
	char szMessage[_MAX_PATH] = {0};
	unsigned long dwLen = nSrcLen;
	 
	ByteCopyN(pszDecrypted, pszEncryptedSrc, nSrcLen);

	if (!CryptDecrypt(*phKey, 0, FALSE, 0, pszDecrypted, &dwLen)) {
		sprintf(szMessage, "Error during CryptDecrypt failed.: 0x%08x", GetLastError());
		HandleError(szMessage);
		bRetVal = FALSE;
	}

	return bRetVal;
}

BOOL AES_Encrypt(HCRYPTKEY* phKey, unsigned const char* lpszSource, 
				 int nSrcLen, unsigned char* pszEncrypted)
{
	BOOL bRetVal = TRUE;
	char szMessage[_MAX_PATH] = {0};
	unsigned long dwLen = nSrcLen;

	ByteCopyN(pszEncrypted, lpszSource, nSrcLen);

	if (!CryptEncrypt(*phKey, 0, FALSE, 0, pszEncrypted, &dwLen, nSrcLen)) {
		sprintf(szMessage,  "Error during CryptEncrypt.: 0x%08x", GetLastError());
		HandleError(szMessage);
		bRetVal = FALSE;
	}

	return bRetVal;
}


// nPaddingType : 0 == Left Padding, 1 == Right Padding
int AES_Initialize(const unsigned char* lpszKey, const int nKeyLen, const unsigned char chPadding,
					int nPaddingType, HCRYPTPROV* phProv, HCRYPTKEY* phKey)
{
	int nRetVal = 0;
	struct {
		BLOBHEADER hdr;
		DWORD cbKeySize; // must be 32(AES_KEY_LENGTH)
		unsigned char rgbKeyData[AES_KEY_LENGTH];
	} keyBlob;

	DWORD dwMode = CRYPT_MODE_CBC;

	char szMessage[_MAX_PATH] = {0};

	keyBlob.hdr.aiKeyAlg = CALG_AES_256;
	keyBlob.hdr.bType = PLAINTEXTKEYBLOB;
	keyBlob.hdr.bVersion = CUR_BLOB_VERSION;
	keyBlob.hdr.reserved = 0;
	keyBlob.cbKeySize = AES_KEY_LENGTH;

	ByteFillChar(keyBlob.rgbKeyData, chPadding, AES_KEY_LENGTH);
	switch(nPaddingType)
	{
	case 0:
		ByteCopyN(keyBlob.rgbKeyData + (AES_KEY_LENGTH - nKeyLen), lpszKey, nKeyLen);
		break;
	case 1:
		ByteCopyN(keyBlob.rgbKeyData, lpszKey, nKeyLen);
		break;
	}

	if (!CryptAcquireContext(phProv, NULL, NULL, // MS_ENH_RSA_AES_PROV,
							PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
 		sprintf(szMessage, "Error during CryptAcquireContext.: 0x%08x", GetLastError());
		HandleError(szMessage);
		
		return nRetVal;
	}

	if (!CryptImportKey(*phProv, (BYTE*)&keyBlob, sizeof(keyBlob), 0, 0, phKey)) {
 		sprintf(szMessage, "Error during CryptImportKey.: 0x%08x", GetLastError());
		HandleError(szMessage);
		
		return nRetVal;
	}

	if (!CryptSetKeyParam(*phKey, KP_MODE, (BYTE*)&dwMode, 0)) {
		sprintf(szMessage, "Error during CryptSetKeyParam.: 0x%08x", GetLastError());
		HandleError(szMessage);
		
		return nRetVal;
	}

	nRetVal  = 1;
	
	return nRetVal;
}

/*
void HandleError(char *s);

int ByteReplace(unsigned char* pszSource, int iLength, char chOld, char chNew, int iStart, int iEnd);

void ByteFillChar(unsigned char* pszByte, char ch, int iLength);

int ByteCopyN(unsigned char* pszDest, const unsigned char* pszSource, int iLength);

void HexToHexString(unsigned long nHexLen, void* pv, char* sz);

unsigned long HexStringToHex(unsigned long nStrLen, const unsigned char* sz, PBYTE szHex);


int AdjustSourceLen(int nSrcLen, int nMultiple);

int AES_Initialize(const unsigned char* lpszKey, const int nKeyLen, const unsigned char chPadding, int nPaddingType, HCRYPTPROV* hProv, HCRYPTKEY* hKey);

BOOL AES_Encrypt(HCRYPTKEY* phKey, unsigned const char* lpszSource, int nLen, unsigned char* pszEncrypted);

BOOL AES_Decrypt(HCRYPTKEY* phKey, unsigned const char* pszEncryptedSrc, int nLen, unsigned char* pszDecrypted);

char* Encrypt(const unsigned char* lpszKey, unsigned long nKeyLen, const unsigned char chPadding, int nPaddingType, const unsigned char* lpszPlain, unsigned long nPlainLen, char* pszEncryptedHexStr);

char* Decrypt(const unsigned char* lpszKey, unsigned long nKeyLen, const unsigned char chPadding, int nPaddingType, const unsigned char* lpszEncHex, unsigned long nEncHexLen, char* pszPlain);


void HandleError(char *sz)
{
	//printf("An error occurred in running the program.\n");
	//printf("%s\n", sz);

	MessageBox(0, TEXT("An error occurred in running the program."), NULL, 0x00000000L);
	
}

int AdjustSourceLen(int nSrcLen, int nMultiple)
{
	int nAdjustLen = 0;
	int nRemainder = 0;

	// adjust size, 16 Multiple
	nRemainder = nSrcLen % nMultiple;
	nAdjustLen = nRemainder ? nMultiple - nRemainder : 0;

	return (nSrcLen + nAdjustLen);
}

void ByteFillChar(unsigned char* pszByte, char ch, int iLength)
{
	int i = 0;

	for (i = 0; i < iLength; i++)
	{
		*pszByte++ = ch;
	}

	return;
}

int ByteCopyN(unsigned char* pszDest, const unsigned char* pszSource, int iLength)
{
	int i = 0;

	for (i = 0; i < iLength; i++)
	{
		*(pszDest + 1) = *(pszSource + i);
	}

	return i;
}

int ByteReplace(unsigned char* pszSource, int iLength, char chOld, char chNew, int iStart, int iEnd)
{
	int idx		= 0;
	int iLast	= 0;
	int	iRepl	= 0;

	iLast = (iEnd == -1) ? iLength : iEnd;

	for (idx = iStart; idx < iLast; idx++)
	{
		if (*(pszSource + idx) == chOld)
		{
			*(pszSource + idx) = chNew;
			++iRepl;
		}
	}

	return iRepl;
}

void HexToHexString(unsigned long nHexLen, void* pv, char* sz)
{
	BYTE* pb = (BYTE*) pv;

	unsigned long i = 0;
	__int16 b;

	for (i = 0; i < nHexLen; i++)
	{
		b = (*pb & 0xF0) >> 4;
		*sz++ = (b <= 9) ? (char)(b + '0') : (char)((b - 10) + 'A');
		b = *pb & 0x0F;
		*sz++ = (b <= 9) ? (char)(b + '0') : (char)((b - 10) + 'A');
		pb++;
	}
	// *sz++ = 0;
}

unsigned long HexStringToHex(unsigned long nStrLen, const unsigned char* sz, PBYTE szHex)
{
	unsigned long iCnt = 0;
	int iRes = 0;
	char ch = 0;

	// lower ==> upper
	int iUp = 0;

	for (iCnt = 0; iCnt < nStrLen; iCnt += 2)
	{
		iUp = (*sz >= 'a') && (*sz <= 'z') ? 10 - 'a': 10 - 'A';
		ch = (*sz > '9') ? (char)(*sz + iUp) : (char)(*sz - '0');
		iRes = (ch << 4);
		sz++;

		iUp = (*sz >= 'a') && (*sz <= 'z') ? 10 - 'a': 10 - 'A';
		ch = (*sz > '9') ? (char)(*sz + iUp) : (char)(*sz - '0');
		iRes += ch;
		sz++;

		*(szHex + iCnt / 2) = (BYTE)iRes;
	}

	return (iCnt / 2);
}

char* Encrypt(const unsigned char* lpszKey, unsigned long nKeyLen, const unsigned char chPadding, int nPaddingType, const unsigned char* lpszPlain, unsigned long nPlainLen, char* pszEncryptedHexStr)
{
	HCRYPTPROV	hProv = 0;
	HCRYPTKEY	hKey = 0;
	HCRYPTKEY	hDuplicateKey = 0;

	unsigned char *pszBuf = NULL;
	unsigned long dwAdjust = 0;
	unsigned long i = 0;

	*pszEncryptedHexStr = 0x00;

	if (!AES_Initialize(lpszKey, nKeyLen, chPadding, nPaddingType, &hProv, &hKey))
	{
		if (pszBuf != NULL)
			LocalFree(pszBuf);
		
		if (hKey != 0)
			CryptDestroyKey(hKey);

		if (hProv != 0)
			CryptReleaseContext(hProv, 0);

		return pszEncryptedHexStr;
	}

	dwAdjust = AdjustSourceLen(nPlainLen, SOURCE_MULTIPLE);
	pszBuf = (unsigned char*)LocalAlloc(LPTR, dwAdjust + 1);

	CryptDuplicateKey(hKey, NULL, 0, &hDuplicateKey);
	for (i = 0; i < dwAdjust; i += BLOCK_SIZE)
		AES_Encrypt(&hDuplicateKey, (const unsigned char*)(lpszPlain + i), BLOCK_SIZE, (unsigned char*)(pszBuf + i));
	CryptDestroyKey(hDuplicateKey);

	HexToHexString(dwAdjust, pszBuf, pszEncryptedHexStr);

	if (pszBuf != NULL)
		LocalFree(pszBuf);

	if (hKey != 0)
		CryptDestroyKey(hKey);

	if (hProv != 0)
		CryptReleaseContext(hProv, 0);

	return pszEncryptedHexStr;

}

char* Decrypt(const unsigned char* lpszKey, unsigned long nKeyLen, const unsigned char chPadding, int nPaddingType, const unsigned char* lpszEncHex, unsigned long nEncHexLen, char* pszPlain)
{
	HCRYPTPROV	hProv = 0;
	HCRYPTKEY	hKey = 0;
	HCRYPTKEY	hDuplicateKey = 0;

	unsigned char *pszBuf = NULL;
	unsigned long dwAdjust = 0;
	unsigned long i = 0;

	*pszPlain = 0x00;

	if (!AES_Initialize(lpszKey, nKeyLen, chPadding, nPaddingType, &hProv, &hKey))
	{
		if (pszBuf != NULL)
			LocalFree(pszBuf);

		if (hKey != 0)
			CryptDestroyKey(hKey);

		if (hProv != 0)
			CryptReleaseContext(hProv, 0);

		return pszPlain;
	}

	dwAdjust = nEncHexLen;
	pszBuf = (unsigned char*)LocalAlloc(LPTR, dwAdjust + 1);

	HexStringToHex(dwAdjust, lpszEncHex, pszBuf);

	CryptDuplicateKey(hKey, NULL, 0, &hDuplicateKey);
	for (i = 0; i < dwAdjust; i += BLOCK_SIZE)
		AES_Decrypt(&hDuplicateKey, (const unsigned char*)(pszBuf + i), BLOCK_SIZE, (unsigned char*)(pszPlain + i));

	CryptDestroyKey(hDuplicateKey);

	if (pszBuf != NULL)
		LocalFree(pszBuf);

	if (hKey != 0)
		CryptDestroyKey(hKey);

	if (hProv != 0)
		CryptReleaseContext(hProv, 0);

	return pszPlain;
}

BOOL AES_Decrypt(HCRYPTKEY* phKey, unsigned const char* pszEncryptedSrc, int nSrcLen, unsigned char* pszDecrypted)
{
	BOOL bRetVal = TRUE;
	char szMessage[_MAX_PATH] = {0};
	unsigned long dwLen = nSrcLen;

	ByteCopyN(pszDecrypted, pszEncryptedSrc, nSrcLen);

	if (!CryptDecrypt(*phKey, 0, FALSE, 0, pszDecrypted, &dwLen))
	{
		sprintf(szMessage, "Error during CryptDecrypt failed : 0x%08x", GetLastError());
		HandleError(szMessage);
		bRetVal = FALSE;
	}

	return bRetVal;
}

BOOL AES_Encrypt(HCRYPTKEY* phKey, unsigned const char* lpszSource, int nSrcLen, unsigned char* pszEncrypted)
{
	BOOL bRetVal = TRUE;
	char szMessage[_MAX_PATH] = {0};
	unsigned long dwLen = nSrcLen;

	ByteCopyN(pszEncrypted, lpszSource, nSrcLen);

	if (!CryptEncrypt(*phKey, 0, FALSE, 0, pszEncrypted, &dwLen, nSrcLen))
	{
		sprintf(szMessage, "Error during CryptEncrypt.: 0x%08x", GetLastError());
		HandleError(szMessage);
		bRetVal = FALSE;
	}

	return bRetVal;
}

// nPaddingType : 0 == Left Padding, 1 == Right Padding
int AES_Initialize(const unsigned char* lpszKey, const int nKeyLen, const unsigned char chPadding, int nPaddingType, HCRYPTPROV* phProv, HCRYPTKEY* phKey)
{
	int nRetVal = 0;

	struct {
		BLOBHEADER hdr;
		DWORD cbKeySize; // must be 32(AES_KEY_LENGTH)
		unsigned char rgbKeyData[AES_KEY_LENGTH];
	} keyBlob;

	DWORD dwMode = CRYPT_MODE_CBC;

	char szMessage[_MAX_PATH] = {0};

	keyBlob.hdr.aiKeyAlg = CALG_AES_256;
	keyBlob.hdr.bType = PLAINTEXTKEYBLOB;
	keyBlob.hdr.bVersion = CUR_BLOB_VERSION;
	keyBlob.hdr.reserved = 0;
	keyBlob.cbKeySize = AES_KEY_LENGTH;

	ByteFillChar(keyBlob.rgbKeyData, chPadding, AES_KEY_LENGTH);
	switch(nPaddingType)
	{
	case 0:
		ByteCopyN(keyBlob.rgbKeyData + (AES_KEY_LENGTH - nKeyLen), lpszKey, nKeyLen);
		break;
	case 1:
		ByteCopyN(keyBlob.rgbKeyData, lpszKey, nKeyLen);
		break;
	}

	if (!CryptAcquireContext(phProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
	{
		sprintf(szMessage, "Error during CryptAcquireContext.: 0x%08x", GetLastError());
		HandleError(szMessage);

		return nRetVal;
	}

	if (!CryptImportKey(*phProv, (BYTE*)&keyBlob, sizeof(keyBlob), 0, 0, phKey))
	{
		sprintf(szMessage, "Error during CryptImportKey.: 0x%08x", GetLastError());
		HandleError(szMessage);

		return nRetVal;
	}

	if (!CryptSetKeyParam(*phKey, KP_MODE, (BYTE*)&dwMode, 0))
	{
		sprintf(szMessage, "Error during CryptSetKeyParam.: 0x%08x", GetLastError());
		HandleError(szMessage);

		return nRetVal;
	}
	
	nRetVal = 1;

	return nRetVal;
}
*/