#pragma once
#include <d3dx9.h>
#include <queue>
class MessNotice
{
public:
	MessNotice(LPDIRECT3DDEVICE9 g_pd3dDevice, int FPS, HWND wnd);
	void push_back(LPCWSTR mess);
	void Draw_mess();
	~MessNotice();
	std::queue<LPCWSTR> all_mess;
	LPDIRECT3DDEVICE9 p_Dev;
	LPD3DXFONT	g_pTextHelper;  // ������Ϣ��2D�ı�
	HWND hwnd;
	int Fps_count;
	int curr_count;
};

