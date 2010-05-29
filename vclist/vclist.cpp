/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// vclist.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "vclist.h"

#include "resource.h"

const char szTitle[] = "Video Codec List";
const char szWindowClass[] = "VCLIST";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hWndMain;
HWND hWndTabArch;;
HWND hWndTabInterface;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HACCEL hAccel;
	MSG msg;

	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wcex);

	hWndMain = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VCLIST));

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
			hWndTabArch = CreateWindow(WC_TABCONTROL, "",
				WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 1, 1,
				hWnd, (HMENU)-1, NULL, NULL); 

			SendMessage(hWndTabInterface, WM_SETFONT, (WPARAM) GetStockObject(ANSI_VAR_FONT), TRUE);
			SendMessage(hWndTabArch, WM_SETFONT, (WPARAM) GetStockObject(ANSI_VAR_FONT), TRUE);

			memset(&item, 0, sizeof(item));
			item.mask = TCIF_TEXT;

			item.pszText = "x86";
			TabCtrl_InsertItem(hWndTabArch, 0, &item);
			item.pszText = "x64";
			TabCtrl_InsertItem(hWndTabArch, 2, &item);

			item.pszText = "VCM";
			TabCtrl_InsertItem(hWndTabInterface, 0, &item);
			item.pszText = "DMO";
			TabCtrl_InsertItem(hWndTabInterface, 1, &item);
			item.pszText = "DirectShow";
			TabCtrl_InsertItem(hWndTabInterface, 2, &item);
		}
		return 0;
	case WM_SIZE:
		{
			RECT rc;

			SetRect(&rc, 0, 0, LOWORD(lParam), HIWORD(lParam));
			MoveWindow(hWndTabArch, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
			TabCtrl_AdjustRect(hWndTabArch, FALSE, &rc);
			MoveWindow(hWndTabInterface, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
			TabCtrl_AdjustRect(hWndTabInterface, FALSE, &rc);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
