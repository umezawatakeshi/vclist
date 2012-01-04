/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// vclist.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "vclist.h"

#include "resource.h"

const char szTitle[] = "Video Codec List";
const char szVersion[] = "1.1.1";
const char szWindowClass[] = "VCLIST";

struct ARCHINFO
{
	char szName[16];
	UINT uIDBase;
};

const ARCHINFO arch_x86 = { "x86", IDC_ARCHITECTURE_BASE_X86 };
const ARCHINFO arch_x64 = { "x64", IDC_ARCHITECTURE_BASE_X64 };

const ARCHINFO *archinfo[3];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Refresh(HWND hWnd);

HWND hWndMain;
HWND hWndTabArch;
HWND hWndTabType;
HWND hWndTabInterface;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HACCEL hAccel;
	MSG msg;
	HMODULE hModuleKernel32;
	SYSTEM_INFO si;
	typedef void (WINAPI *pfn_gsi_t)(SYSTEM_INFO *);
	pfn_gsi_t gsi;
	INITCOMMONCONTROLSEX iccex;

	iccex.dwSize = sizeof(iccex);
	iccex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&iccex);

	hModuleKernel32 = LoadLibrary("KERNEL32.DLL");
	gsi = (pfn_gsi_t)GetProcAddress(hModuleKernel32, "GetNativeSystemInfo");
	if (gsi == NULL)
		gsi = GetSystemInfo;
	gsi(&si);
	switch(si.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
	default:
		archinfo[0] = &arch_x86;
		archinfo[1] = NULL;
		break;
	case PROCESSOR_ARCHITECTURE_AMD64:
		archinfo[0] = &arch_x86;
		archinfo[1] = &arch_x64;
		archinfo[2] = NULL;
		break;
	}

	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName  = MAKEINTRESOURCE(IDR_VCLIST);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wcex);

	hWndMain = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_VCLIST));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hWndMain, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			TCITEM item;

			hWndTabInterface = CreateWindow(WC_TABCONTROL, "",
				WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 1, 1,
				hWnd, (HMENU)-1, NULL, NULL); 
			hWndTabType = CreateWindow(WC_TABCONTROL, "",
				WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 1, 1,
				hWnd, (HMENU)-1, NULL, NULL); 
			hWndTabArch = CreateWindow(WC_TABCONTROL, "",
				WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 1, 1,
				hWnd, (HMENU)-1, NULL, NULL); 

			SendMessage(hWndTabInterface, WM_SETFONT, (WPARAM) GetStockObject(ANSI_VAR_FONT), TRUE);
			SendMessage(hWndTabType, WM_SETFONT, (WPARAM) GetStockObject(ANSI_VAR_FONT), TRUE);
			SendMessage(hWndTabArch, WM_SETFONT, (WPARAM) GetStockObject(ANSI_VAR_FONT), TRUE);

			memset(&item, 0, sizeof(item));

			item.mask = TCIF_TEXT | TCIF_PARAM;
			item.pszText = "VCM";
			item.lParam = IDC_INTERFACE_OFFSET_VCM;
			TabCtrl_InsertItem(hWndTabInterface, 0, &item);
			item.pszText = "DMO";
			item.lParam = IDC_INTERFACE_OFFSET_DMO;
			TabCtrl_InsertItem(hWndTabInterface, 1, &item);
			item.pszText = "DirectShow";
			item.lParam = IDC_INTERFACE_OFFSET_DSF;
			TabCtrl_InsertItem(hWndTabInterface, 2, &item);

			item.mask = TCIF_TEXT | TCIF_PARAM;
			item.pszText = "Encoder";
			item.lParam = IDC_TYPE_OFFSET_ENCODER;
			TabCtrl_InsertItem(hWndTabType, 0, &item);
			item.pszText = "Decoder";
			item.lParam = IDC_TYPE_OFFSET_DECODER;
			TabCtrl_InsertItem(hWndTabType, 1, &item);

			item.mask = TCIF_TEXT | TCIF_PARAM;
			for (int i = 0; archinfo[i] != NULL; i++)
			{
				char buf[sizeof(archinfo[i]->szName)];
				LVCOLUMN col;
				HWND hWndListView;

				strcpy(buf, archinfo[i]->szName);
				item.pszText = buf;
				item.lParam  = archinfo[i]->uIDBase;
				TabCtrl_InsertItem(hWndTabArch, i, &item);

				memset(&col, 0, sizeof(col));
				col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

				for (UINT uIDType = 0; uIDType < IDC_TYPE_OFFSET_END; uIDType += IDC_TYPE_OFFSET_STEP)
				{
					hWndListView = CreateWindow(WC_LISTVIEW, "",
						WS_CHILD | LVS_REPORT | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 1, 1,
						hWnd, (HMENU)(archinfo[i]->uIDBase + uIDType + IDC_INTERFACE_OFFSET_VCM), NULL, NULL);
					ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT);

					col.fmt = LVCFMT_LEFT;
					col.cx = 300;
					col.pszText = "Name";
					col.iSubItem = 0;
					ListView_InsertColumn(hWndListView, 0, &col);

					col.fmt = LVCFMT_LEFT;
					col.cx = 100;
					col.pszText = "FourCC";
					col.iSubItem = 1;
					ListView_InsertColumn(hWndListView, 1, &col);

					hWndListView = CreateWindow(WC_LISTVIEW, "",
						WS_CHILD | LVS_REPORT | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 1, 1,
						hWnd, (HMENU)(archinfo[i]->uIDBase + uIDType + IDC_INTERFACE_OFFSET_DMO), NULL, NULL);
					ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT);

					col.fmt = LVCFMT_LEFT;
					col.cx = 300;
					col.pszText = "Name";
					col.iSubItem = 0;
					ListView_InsertColumn(hWndListView, 0, &col);

					col.fmt = LVCFMT_LEFT;
					col.cx = 300;
					col.pszText = "CLSID";
					col.iSubItem = 1;
					ListView_InsertColumn(hWndListView, 1, &col);

					hWndListView = CreateWindow(WC_LISTVIEW, "",
						WS_CHILD | LVS_REPORT | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 1, 1,
						hWnd, (HMENU)(archinfo[i]->uIDBase + uIDType + IDC_INTERFACE_OFFSET_DSF), NULL, NULL);
					ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT);

					col.fmt = LVCFMT_LEFT;
					col.cx = 300;
					col.pszText = "Name";
					col.iSubItem = 0;
					ListView_InsertColumn(hWndListView, 0, &col);

					col.fmt = LVCFMT_LEFT;
					col.cx = 700;
					col.pszText = "DisplayName";
					col.iSubItem = 1;
					ListView_InsertColumn(hWndListView, 1, &col);
				}
			}
			BringWindowToTop(GetDlgItem(hWnd, archinfo[0]->uIDBase));

			Refresh(hWnd);
		}
		return 0;
	case WM_SIZE:
		{
			RECT rc;

			SetRect(&rc, 0, 0, LOWORD(lParam), HIWORD(lParam));
			MoveWindow(hWndTabArch, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
			TabCtrl_AdjustRect(hWndTabArch, FALSE, &rc);
			MoveWindow(hWndTabType, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
			TabCtrl_AdjustRect(hWndTabType, FALSE, &rc);
			MoveWindow(hWndTabInterface, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
			TabCtrl_AdjustRect(hWndTabInterface, FALSE, &rc);
			for (int i = 0; archinfo[i] != NULL; i++)
			{
				for (int j = 0; j < IDC_INTERFACE_OFFSET_END; j++)
					for (int k = 0; k < IDC_TYPE_OFFSET_END; k += IDC_TYPE_OFFSET_STEP)
						MoveWindow(GetDlgItem(hWnd, archinfo[i]->uIDBase + k + j), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
			}
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;
		case ID_VIEW_REFRESH:
			Refresh(hWnd);
			return 0;
		case ID_HELP_ABOUT:
			{
				char buf[256];
				wsprintf(buf,
					"%s, version %s\n"
					"Copyright (C) 2010-2012  UMEZAWA Takeshi\n\n"
					"Licensed under GNU General Public License version 2 or later.",
					szTitle, szVersion);
				MessageBox(hWnd, buf, szTitle, 0);
			}
			return 0;
		}
		return 0;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case TCN_SELCHANGE:
			{
				TCITEM item;
				UINT uID;

				memset(&item, 0, sizeof(item));
				item.mask = TCIF_PARAM;
				TabCtrl_GetItem(hWndTabArch, TabCtrl_GetCurSel(hWndTabArch), &item);
				uID = (UINT)item.lParam;
				TabCtrl_GetItem(hWndTabType, TabCtrl_GetCurSel(hWndTabType), &item);
				uID += (UINT)item.lParam;
				TabCtrl_GetItem(hWndTabInterface, TabCtrl_GetCurSel(hWndTabInterface), &item);
				uID += (UINT)item.lParam;
				BringWindowToTop(GetDlgItem(hWnd, uID));
			}
			return TRUE;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void _cdecl DoRefresh(void *lpvParam)
{
	HWND hWnd = (HWND)lpvParam;
	char szAppDir[256];

	GetModuleFileName(NULL, szAppDir, _countof(szAppDir));
	*strrchr(szAppDir, '\\') = '\0';

	for (int i = 0; archinfo[i] != NULL; i++)
	{
		char szAppName[256];
		HANDLE hStdOut, hStdOutParentSide;
		SECURITY_ATTRIBUTES sa;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		for (int j = 0; j < IDC_INTERFACE_OFFSET_END; j++)
			for (int k = 0; k < IDC_TYPE_OFFSET_END; k += IDC_TYPE_OFFSET_STEP)
				ListView_DeleteAllItems(GetDlgItem(hWnd, archinfo[i]->uIDBase + k + j));
		wsprintf(szAppName, "%s\\vclist-enum-%s.exe", szAppDir, archinfo[i]->szName);

		memset(&sa, 0, sizeof(sa));
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		CreatePipe(&hStdOutParentSide, &hStdOut, &sa, 0);
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdOutput = hStdOut;

		if (CreateProcess(szAppName, "", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		{
			char buf[2048];
			FILE *fp;

			CloseHandle(hStdOut);
			fp = _fdopen( _open_osfhandle((intptr_t)hStdOutParentSide, _O_RDONLY), "rt");

			while (fgets(buf, 2000, fp) != NULL)
			{
				LVITEM item;
				UINT uID;
				HWND hWndListView;
				char *iface;
				char *type;
				char *name;
				char *aux;

				strtok(buf, "\r\n"); // 末尾の改行文字を削除する簡単な方法。
				iface = buf;

				uID = archinfo[i]->uIDBase;

				type = strchr(iface, '\t');
				*type++ = '\0';
				name = strchr(type, '\t');
				*name++ = '\0';
				aux = strchr(name, '\t');
				*aux++ = '\0';

				if (strcmp(iface, "VCM") == 0)
					uID += IDC_INTERFACE_OFFSET_VCM;
				else if (strcmp(iface, "DMO") == 0)
					uID += IDC_INTERFACE_OFFSET_DMO;
				else if (strcmp(iface, "DSF") == 0)
					uID += IDC_INTERFACE_OFFSET_DSF;
				else
					continue;

				if (strcmp(type, "1") == 0)
					uID += IDC_TYPE_OFFSET_ENCODER;
				else
					uID += IDC_TYPE_OFFSET_DECODER;

				hWndListView = GetDlgItem(hWnd, uID);
				memset(&item, 0, sizeof(item));
				item.mask = LVIF_TEXT;
				item.iItem = ListView_GetItemCount(hWndListView);
				item.pszText = name;
				ListView_InsertItem(hWndListView, &item);

				item.iSubItem = 1;
				item.pszText = aux;
				ListView_SetItem(hWndListView, &item);
			}

			fclose(fp);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		else
		{
			char buf[256];

			CloseHandle(hStdOut);
			CloseHandle(hStdOutParentSide);

			sprintf(buf, "Cannot execute enumerator program:\n%s", szAppName);
			MessageBox(hWnd, buf, szTitle, MB_ICONERROR);
		}
	}
}

void Refresh(HWND hWnd)
{
	_beginthread(DoRefresh, 0, (LPVOID)hWnd);
}
