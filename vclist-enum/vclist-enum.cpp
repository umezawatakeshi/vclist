/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// vclist-enum.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define INITGUID
#include <guiddef.h>

DEFINE_GUID(MFT_CATEGORY_VIDEO_DECODER,
0xd6c02d4b, 0x6833, 0x45b4, 0x97, 0x1a, 0x05, 0xa4, 0xb0, 0x4b, 0xab, 0x91);
DEFINE_GUID(MFT_CATEGORY_VIDEO_ENCODER,
0xf79eac7d, 0xe545, 0x4387, 0xbd, 0xee, 0xd6, 0x47, 0xd7, 0xbd, 0xe4, 0x2a);

#pragma warning(disable : 4995)

#define FCC4PRINTF(fcc) \
	(BYTE)(fcc), \
	(BYTE)(fcc >> 8), \
	(BYTE)(fcc >> 16), \
	(BYTE)(fcc >> 24)

void EnumVCM(bool bEnc);
void EnumDMO(bool bEnc);
void EnumDSF(void);
void EnumMFT(bool bEnc);

/*
 * WinMain から始まる GUI アプリケーションであり、
 * printf してもコンソールウィンドウが出てきたりはしない。
 * コマンドプロンプトから実行しても（なぜか）何も表示されない。
 */

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	setvbuf(stdout, NULL, _IONBF, 0);

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	EnumVCM(1);
	EnumVCM(0);
	EnumDMO(1);
	EnumDMO(0);
	EnumDSF();
	EnumMFT(1);
	EnumMFT(0);

	return 0;
}

void EnumVCM(bool bEnc)
{
	ICINFO info;
	char buf[256];

	for (int i = 0; ICInfo(ICTYPE_VIDEO, i, &info); i++)
	{
		HIC hic = ICOpen(ICTYPE_VIDEO, info.fccHandler, bEnc ? ICMODE_COMPRESS : ICMODE_DECOMPRESS);
		if (hic == NULL)
			continue;
		ICGetInfo(hic, &info, sizeof(info));
		ICClose(hic);
		wsprintf(buf, "VCM\t%d\t%S\t%c%c%c%c\n", bEnc, info.szDescription, FCC4PRINTF(info.fccHandler));
		printf("%s", buf);
	}
}

void EnumDMO(bool bEnc)
{
	char buf[256];
	WCHAR wbuf[256];
	WCHAR *pwsz;
	IEnumDMO *pEnumDMO;
	CLSID clsid;
	DWORD dwCount;

	DMOEnum(bEnc ? DMOCATEGORY_VIDEO_ENCODER : DMOCATEGORY_VIDEO_DECODER, DMO_ENUMF_INCLUDE_KEYED, 0, NULL, 0, NULL, &pEnumDMO);
	while (pEnumDMO->Next(1, &clsid, &pwsz, &dwCount) == S_OK)
	{
		StringFromGUID2(clsid, wbuf, _countof(wbuf));
		wsprintf(buf, "DMO\t%d\t%S\t%S\n", bEnc, pwsz, wbuf);
		printf("%s", buf);
		CoTaskMemFree(pwsz);
	}
	pEnumDMO->Release();
}

void EnumDSF(void)
{
	ICreateDevEnum *pCreateDevEnum;
	IEnumMoniker *pEnumMoniker;
	IMoniker *pMoniker;
	ULONG nFetched;
	char buf[256];

	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (PVOID *)&pCreateDevEnum);

	if (pCreateDevEnum->CreateClassEnumerator(CLSID_VideoCompressorCategory, &pEnumMoniker, 0) != S_OK)
	{
		pCreateDevEnum->Release();
		return;
	}

	pEnumMoniker->Reset();

	while (pEnumMoniker->Next(1, &pMoniker, &nFetched) == S_OK)
	{
		IPropertyBag *pPropertyBag;
		VARIANT varFriendlyName;
		WCHAR *pwszDisplayName;

		pMoniker->GetDisplayName(NULL, NULL, &pwszDisplayName);
		varFriendlyName.vt = VT_BSTR;
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropertyBag);
		pPropertyBag->Read(L"FriendlyName", &varFriendlyName, 0);
		wsprintf(buf, "DSF\t1\t%S\t%S\n", varFriendlyName.bstrVal, pwszDisplayName);
		printf("%s", buf);
		VariantClear(&varFriendlyName);
		CoTaskMemFree(pwszDisplayName);
		pPropertyBag->Release();
		pMoniker->Release();
	}

	pEnumMoniker->Release();
	pCreateDevEnum->Release();
}

void EnumMFT(bool bEnc)
{
	CLSID *pclsid;
	UINT32 cclsid = 0;
	WCHAR wbuf[256];
	WCHAR *pwsz;
	char buf[256];

	typedef HRESULT (STDAPICALLTYPE *tfnMFTEnum)(GUID, UINT32, MFT_REGISTER_TYPE_INFO *, MFT_REGISTER_TYPE_INFO *, IMFAttributes *, CLSID **, UINT32 *);
	typedef HRESULT (STDAPICALLTYPE *tfnMFTGetInfo)(CLSID, LPWSTR *, MFT_REGISTER_TYPE_INFO **, UINT32 *, MFT_REGISTER_TYPE_INFO **, UINT32 *, IMFAttributes **);

	tfnMFTEnum pfnMFTEnum;
	tfnMFTGetInfo pfnMFTGetInfo;
	HMODULE hLib;

	if (LOBYTE(LOWORD(GetVersion())) >= 6 /* Windows Vista */)
	{
		hLib = LoadLibrary("mfplat.dll");
		pfnMFTEnum = (tfnMFTEnum)GetProcAddress(hLib, "MFTEnum");
		pfnMFTGetInfo = (tfnMFTGetInfo)GetProcAddress(hLib, "MFTGetInfo");

		pfnMFTEnum(bEnc ? MFT_CATEGORY_VIDEO_ENCODER : MFT_CATEGORY_VIDEO_DECODER, 0, NULL, NULL, NULL, &pclsid, &cclsid);
		for (UINT32 i = 0; i < cclsid; i++) 
		{
			pfnMFTGetInfo(pclsid[i], &pwsz, NULL, NULL, NULL, NULL, NULL);
			StringFromGUID2(pclsid[i], wbuf, _countof(wbuf));
			wsprintf(buf, "MFT\t%d\t%S\t%S\n", bEnc, pwsz, wbuf);
			printf("%s", buf);
			CoTaskMemFree(pwsz);
		}
		CoTaskMemFree(pclsid);
	}
}
