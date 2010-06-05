/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// vclist-enum.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define FCC4PRINTF(fcc) \
	(BYTE)(fcc), \
	(BYTE)(fcc >> 8), \
	(BYTE)(fcc >> 16), \
	(BYTE)(fcc >> 24)

void EnumVCM(void);
void EnumDMO(void);
void EnumDSF(void);

/*
 * WinMain から始まる GUI アプリケーションであり、
 * printf してもコンソールウィンドウが出てきたりはしない。
 * コマンドプロンプトから実行しても（なぜか）何も表示されない。
 */

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	EnumVCM();
	EnumDMO();
	EnumDSF();

	return 0;
}

void EnumVCM(void)
{
	ICINFO info;
	char buf[256];

	for (int i = 0; ICInfo(ICTYPE_VIDEO, i, &info); i++)
	{
		HIC hic = ICOpen(ICTYPE_VIDEO, info.fccHandler, ICMODE_COMPRESS);
		if (hic == NULL)
			continue;
		ICGetInfo(hic, &info, sizeof(info));
		ICClose(hic);
		wsprintf(buf, "VCM\t%S\t%c%c%c%c\n", info.szDescription, FCC4PRINTF(info.fccHandler));
		printf("%s", buf);
	}
}

void EnumDMO(void)
{
	char buf[256];
	WCHAR wbuf[256];
	WCHAR *pwsz;
	IEnumDMO *pEnumDMO;
	CLSID clsid;
	DWORD dwCount;

	DMOEnum(DMOCATEGORY_VIDEO_ENCODER, DMO_ENUMF_INCLUDE_KEYED, 0, NULL, 0, NULL, &pEnumDMO);
	while (pEnumDMO->Next(1, &clsid, &pwsz, &dwCount) == S_OK)
	{
		StringFromGUID2(clsid, wbuf, _countof(wbuf));
		wsprintf(buf, "DMO\t%S\t%S\n", pwsz, wbuf);
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
		wsprintf(buf, "DSF\t%S\t%S\n", varFriendlyName.bstrVal, pwszDisplayName);
		printf("%s", buf);
		VariantClear(&varFriendlyName);
		CoTaskMemFree(pwszDisplayName);
		pPropertyBag->Release();
		pMoniker->Release();
	}

	pEnumMoniker->Release();
	pCreateDevEnum->Release();
}
