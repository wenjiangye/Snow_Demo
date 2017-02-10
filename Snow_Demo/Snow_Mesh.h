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
	LPD3DXMESH  Head;  //球体
	LPD3DXMESH  Body;  //圆柱
	LPD3DXMESH  LLeg;   //长方体
	LPD3DXMESH  RLeg;   //长方体
	LPDIRECT3DDEVICE9  pDev;
};

