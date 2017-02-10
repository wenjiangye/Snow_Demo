#include "MessNotice.h"
#include <iostream>


MessNotice::MessNotice(LPDIRECT3DDEVICE9 g_pd3dDevice, int FPS, HWND wnd)
{
	p_Dev = g_pd3dDevice;
	hwnd = wnd;
	Fps_count = FPS;
	curr_count = 0;
	g_pTextHelper = NULL;
	D3DXCreateFont(p_Dev, 23, 0, 1000, 0, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Î¢ÈíÑÅºÚ", &g_pTextHelper);
}
void MessNotice::push_back(LPCWSTR mess)
{
	all_mess.push(mess);
}
void MessNotice::Draw_mess()
{
	if (all_mess.size() == 0)
		return;
	else
	{

		if (curr_count == Fps_count)
		{
			curr_count = 0;
			all_mess.pop();
			return;
		}
		else
		{
			//if (all_mess.size() == 0)
			//	return;
			curr_count++;
			RECT formatRect;
			GetClientRect(hwnd, &formatRect);
			formatRect.left = 500, formatRect.top = 500;
			g_pTextHelper->DrawText(NULL, all_mess.front(), -1, &formatRect,
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
		}


	}
}

MessNotice::~MessNotice()
{
}
