#pragma once
#include <d3dx9.h>
#include "D3DUtil.h"
struct SkyBoxVertex
{
	float x, y, z;  //定点位置  
	float u, v;     //纹理位置  
};
#define D3DFVF_SKYBOX D3DFVF_XYZ | D3DFVF_TEX1  


class CSkyBox
{
private:
	LPDIRECT3DDEVICE9 m_pDevice;            //设备指针  
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;//定点缓冲区指针  
	LPDIRECT3DTEXTURE9 m_pTexture[5];       //纹理对象指针  
	float m_fLength;                        //天空盒宽度  
public:
	CSkyBox(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CSkyBox(void);

	//初始化天空盒  
	bool InitSkyBox(float length);

	//加载纹理  
	bool InitSkyBoxTexture(LPCWSTR szFrontTexture, LPCWSTR szBackTexture, LPCWSTR szLeftTexture, LPCWSTR szRightTexture, LPCWSTR szTopTexture);

	//渲染天空盒  
	void RenderSkyBox(D3DXMATRIX *matWorld, bool bRenderFrame = false);
};

