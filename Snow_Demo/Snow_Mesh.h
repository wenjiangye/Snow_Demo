#pragma once
#include <d3d9.h>
#include <d3dx9.h>
class Snow_Mesh
{
public:
	Snow_Mesh(LPDIRECT3DDEVICE9 pDev);
	void Draw_Snow_body();
	~Snow_Mesh();
private:
	LPD3DXMESH  Head;  //����
	LPD3DXMESH  Body;  //Բ��
	LPD3DXMESH  LLeg;   //������
	LPD3DXMESH  RLeg;   //������
	LPDIRECT3DDEVICE9  pDev;
};

