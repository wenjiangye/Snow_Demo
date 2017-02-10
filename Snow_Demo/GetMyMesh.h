#pragma once
#include <d3dx9.h>
struct VERTEX;

#define FVF_VERTEX D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1

LPD3DXMESH GenerateBoxMesh(LPDIRECT3DDEVICE9 pDev, float a, float b, float c);


LPD3DXMESH GenerateSphereMesh(LPDIRECT3DDEVICE9 pDev,
	float fRadius, UINT slices, UINT stacks);


LPD3DXMESH GenerateCylinderMesh(LPDIRECT3DDEVICE9 pDev, float Radius1, float Radius2, float high, UINT slices, UINT stacks);
