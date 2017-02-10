#include "SkyBox.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice = pDevice;
	m_pVertexBuffer = NULL;
	for (int i = 0; i < 5; i++)
		m_pTexture[i] = NULL;
	m_fLength = 0.0f;
}


CSkyBox::~CSkyBox(void)
{
	SAFE_RELEASE(m_pVertexBuffer);
	for (int i = 0; i < 5; i++)
	{
		SAFE_RELEASE(m_pTexture[i]);
	}
}

bool CSkyBox::InitSkyBox(float length)
{
	m_fLength = length;

	//�������㻺����  
	m_pDevice->CreateVertexBuffer(20 * sizeof(SkyBoxVertex), 0, D3DFVF_SKYBOX, D3DPOOL_MANAGED, &m_pVertexBuffer, 0);

	SkyBoxVertex vertices[] =
	{
		//ǰ��Ķ���  
		{ -m_fLength / 2, 0.0f, m_fLength / 2, 0.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, m_fLength / 2, 0.0f, 0.0f, },
		{ m_fLength / 2, 0.0f, m_fLength / 2, 1.0f, 1.0f, },
		{ m_fLength / 2, m_fLength / 2, m_fLength / 2,  1.0f, 0.0f, },

		//����Ķ���  
		{ m_fLength / 2, 0.0f, -m_fLength / 2, 0.0f, 1.0f, },
		{ m_fLength / 2, m_fLength / 2, -m_fLength / 2, 0.0f, 0.0f, },
		{ -m_fLength / 2, 0.0f, -m_fLength / 2, 1.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, -m_fLength / 2, 1.0f, 0.0f, },

		//����Ķ���  
		{ -m_fLength / 2, 0.0f, -m_fLength / 2 , 0.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, -m_fLength / 2, 0.0f, 0.0f, },
		{ -m_fLength / 2, 0.0f, m_fLength / 2, 1.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, m_fLength / 2, 1.0f, 0.0f, },

		//����Ķ���  
		{ m_fLength / 2, 0.0f, m_fLength / 2, 0.0f, 1.0f, },
		{ m_fLength / 2, m_fLength / 2, m_fLength / 2, 0.0f, 0.0f, },
		{ m_fLength / 2, 0.0f, -m_fLength / 2, 1.0f, 1.0f, },
		{ m_fLength / 2, m_fLength / 2, -m_fLength / 2, 1.0f, 0.0f, },

		//����Ķ���  
		{ m_fLength / 2, m_fLength / 2, -m_fLength / 2, 1.0f, 0.0f, },
		{ m_fLength / 2, m_fLength / 2, m_fLength / 2, 1.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, -m_fLength / 2, 0.0f, 0.0f, },
		{ -m_fLength / 2, m_fLength / 2, m_fLength / 2, 0.0f, 1.0f, },
	};
	//��䶨�㻺������  
	void* pVertices = NULL;
	//����  
	m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	//�����������ݵ���������  
	memcpy(pVertices, vertices, sizeof(vertices));
	//����  
	m_pVertexBuffer->Unlock();



	return true;
}

bool CSkyBox::InitSkyBoxTexture(LPCWSTR szFrontTexture, LPCWSTR szBackTexture, LPCWSTR szLeftTexture, LPCWSTR szRightTexture, LPCWSTR szTopTexture)
{
	//���ļ��м�����պ�����  
	D3DXCreateTextureFromFile(m_pDevice, szFrontTexture, &m_pTexture[0]);//ǰ  
	D3DXCreateTextureFromFile(m_pDevice, szBackTexture, &m_pTexture[1]);//��  
	D3DXCreateTextureFromFile(m_pDevice, szLeftTexture, &m_pTexture[2]);//��  
	D3DXCreateTextureFromFile(m_pDevice, szRightTexture, &m_pTexture[3]);//��  
	D3DXCreateTextureFromFile(m_pDevice, szTopTexture, &m_pTexture[4]);//��  

	return true;
}

void CSkyBox::RenderSkyBox(D3DXMATRIX *matWorld, bool bRenderFrame /* = false */)
{
	//����任  
	m_pDevice->SetTransform(D3DTS_WORLD, matWorld);
	//������Դ��  
	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SkyBoxVertex));
	//���������ʽ  
	m_pDevice->SetFVF(D3DFVF_SKYBOX);

	//����  
	for (int i = 0; i < 5; i++)
	{
		m_pDevice->SetTexture(0, m_pTexture[i]);
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
	}
}