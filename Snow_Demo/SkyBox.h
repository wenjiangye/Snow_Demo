#pragma once
#include <d3dx9.h>
#include "D3DUtil.h"
struct SkyBoxVertex
{
	float x, y, z;  //����λ��  
	float u, v;     //����λ��  
};
#define D3DFVF_SKYBOX D3DFVF_XYZ | D3DFVF_TEX1  


class CSkyBox
{
private:
	LPDIRECT3DDEVICE9 m_pDevice;            //�豸ָ��  
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;//���㻺����ָ��  
	LPDIRECT3DTEXTURE9 m_pTexture[5];       //�������ָ��  
	float m_fLength;                        //��պп��  
public:
	CSkyBox(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CSkyBox(void);

	//��ʼ����պ�  
	bool InitSkyBox(float length);

	//��������  
	bool InitSkyBoxTexture(LPCWSTR szFrontTexture, LPCWSTR szBackTexture, LPCWSTR szLeftTexture, LPCWSTR szRightTexture, LPCWSTR szTopTexture);

	//��Ⱦ��պ�  
	void RenderSkyBox(D3DXMATRIX *matWorld, bool bRenderFrame = false);
};

