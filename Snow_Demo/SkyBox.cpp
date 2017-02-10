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

	//创建定点缓冲区  
	m_pDevice->CreateVertexBuffer(20 * sizeof(SkyBoxVertex), 0, D3DFVF_SKYBOX, D3DPOOL_MANAGED, &m_pVertexBuffer, 0);

	SkyBoxVertex vertices[] =
	{
		//前面的顶点  
		{ -m_fLength / 2, 0.0f, m_fLength / 2, 0.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, m_fLength / 2, 0.0f, 0.0f, },
		{ m_fLength / 2, 0.0f, m_fLength / 2, 1.0f, 1.0f, },
		{ m_fLength / 2, m_fLength / 2, m_fLength / 2,  1.0f, 0.0f, },

		//后面的顶点  
		{ m_fLength / 2, 0.0f, -m_fLength / 2, 0.0f, 1.0f, },
		{ m_fLength / 2, m_fLength / 2, -m_fLength / 2, 0.0f, 0.0f, },
		{ -m_fLength / 2, 0.0f, -m_fLength / 2, 1.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, -m_fLength / 2, 1.0f, 0.0f, },

		//左面的顶点  
		{ -m_fLength / 2, 0.0f, -m_fLength / 2 , 0.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, -m_fLength / 2, 0.0f, 0.0f, },
		{ -m_fLength / 2, 0.0f, m_fLength / 2, 1.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, m_fLength / 2, 1.0f, 0.0f, },

		//右面的顶点  
		{ m_fLength / 2, 0.0f, m_fLength / 2, 0.0f, 1.0f, },
		{ m_fLength / 2, m_fLength / 2, m_fLength / 2, 0.0f, 0.0f, },
		{ m_fLength / 2, 0.0f, -m_fLength / 2, 1.0f, 1.0f, },
		{ m_fLength / 2, m_fLength / 2, -m_fLength / 2, 1.0f, 0.0f, },

		//上面的顶点  
		{ m_fLength / 2, m_fLength / 2, -m_fLength / 2, 1.0f, 0.0f, },
		{ m_fLength / 2, m_fLength / 2, m_fLength / 2, 1.0f, 1.0f, },
		{ -m_fLength / 2, m_fLength / 2, -m_fLength / 2, 0.0f, 0.0f, },
		{ -m_fLength / 2, m_fLength / 2, m_fLength / 2, 0.0f, 1.0f, },
	};
	//填充定点缓冲数据  
	void* pVertices = NULL;
	//加锁  
	m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	//拷贝顶点数据到缓冲区中  
	memcpy(pVertices, vertices, sizeof(vertices));
	//解锁  
	m_pVertexBuffer->Unlock();



	return true;
}

bool CSkyBox::InitSkyBoxTexture(LPCWSTR szFrontTexture, LPCWSTR szBackTexture, LPCWSTR szLeftTexture, LPCWSTR szRightTexture, LPCWSTR szTopTexture)
{
	//从文件中加载天空盒纹理  
	D3DXCreateTextureFromFile(m_pDevice, szFrontTexture, &m_pTexture[0]);//前  
	D3DXCreateTextureFromFile(m_pDevice, szBackTexture, &m_pTexture[1]);//后  
	D3DXCreateTextureFromFile(m_pDevice, szLeftTexture, &m_pTexture[2]);//左  
	D3DXCreateTextureFromFile(m_pDevice, szRightTexture, &m_pTexture[3]);//右  
	D3DXCreateTextureFromFile(m_pDevice, szTopTexture, &m_pTexture[4]);//上  

	return true;
}

void CSkyBox::RenderSkyBox(D3DXMATRIX *matWorld, bool bRenderFrame /* = false */)
{
	//世界变换  
	m_pDevice->SetTransform(D3DTS_WORLD, matWorld);
	//设置资源流  
	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SkyBoxVertex));
	//设置灵活顶点格式  
	m_pDevice->SetFVF(D3DFVF_SKYBOX);

	//绘制  
	for (int i = 0; i < 5; i++)
	{
		m_pDevice->SetTexture(0, m_pTexture[i]);
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
	}
}