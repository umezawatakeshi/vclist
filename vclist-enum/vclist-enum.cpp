/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// vclist-enum.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

/*
 * WinMain から始まる GUI アプリケーションであり、
 * printf してもコンソールウィンドウが出てきたりはしない。
 * コマンドプロンプトから実行しても（なぜか）何も表示されない。
 */

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	printf("abcdefghijklmnopqrstuvwxyz");

	return 0;
}
