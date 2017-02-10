
#define SCREEN_WIDTH	1440						//Ϊ���ڿ�ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ��
#define SCREEN_HEIGHT	760							//Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�
#define WINDOW_TITLE	_T("����ѩ�˳���ʵ��") //Ϊ���ڱ��ⶨ��ĺ�
                                                                                      
#include <d3d9.h>
#include <iostream>
#include <d3dx9.h>
#include <tchar.h>
#include <time.h> 
#include "DirectInputClass.h"
#include "CameraClass.h"
#include "GetMyMesh.h"
#include "Snow_Mesh.h"
#include "SkyBox.h"
#include "MessNotice.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "dinput8.lib")     // ʹ��DirectInput��������Ŀ��ļ���ע��������8
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "winmm.lib") 


//���嶥��ṹ��
struct CUSTOMVERTEX
{
	FLOAT _x,  _y,  _z;
	FLOAT _nx, _ny, _nz;
	FLOAT _u,  _v;
	CUSTOMVERTEX(FLOAT x, FLOAT y, FLOAT z, 
		FLOAT nx, FLOAT ny, FLOAT nz, FLOAT u, FLOAT v)
	{
		_x  = x,  _y  = y,  _z  = z;
		_nx = nx, _ny = ny, _nz = nz;
		_u  = u,  _v  = v;
	}
};
#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)


//*****************************************************************************************
// Desc: ȫ�ֱ�����������  
//*****************************************************************************************
LPDIRECT3DDEVICE9					g_pd3dDevice = NULL; //Direct3D�豸����
LPD3DXFONT								g_pTextFPS				=NULL;    //����COM�ӿ�
LPD3DXFONT								g_pTextHelper          = NULL;  // ������Ϣ��2D�ı�
LPD3DXFONT								g_pTextInfor           = NULL;  // ������Ϣ��2D�ı�
float								g_FPS		= 0.0f;       //һ�������͵ı���������֡����
wchar_t										g_strFPS[50]={0};    //����֡���ʵ��ַ�����
D3DXMATRIX							g_matWorld;   //�������
DInputClass*								g_pDInput = NULL;         //һ��DInputClass���ָ��
CameraClass*							g_pCamera  = NULL;
//
LPD3DXMESH							g_sphere = NULL;     //���������
LPD3DXMESH							g_box = NULL;     //�����������
//
LPDIRECT3DVERTEXBUFFER9		g_pVertexBuffer           = NULL;  //���ƲݵصĶ��㻺�����
LPDIRECT3DTEXTURE9				g_pTexture             = NULL;  //���Ʋݵص��������
LPDIRECT3DTEXTURE9				g_pTexture_snow = NULL;  //����ѩ�˵��������
D3DMATERIAL9					white;
HWND hwnd = NULL;
Snow_Mesh*  snow1 = NULL;
Snow_Mesh*  snow2 = NULL;
CSkyBox*      g_pSkyBox = NULL;    //��պ���ָ��  
MessNotice*   Mess_notice = NULL;
D3DXVECTOR3 Box_pos;
bool LoginFlag = false;


LRESULT CALLBACK		WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
HRESULT						Direct3D_Init(HWND hwnd,HINSTANCE hInstance);
HRESULT						Objects_Init();
void								Direct3D_Render( HWND hwnd);
void								Direct3D_Update( HWND hwnd);
void								Direct3D_CleanUp( );
float								Get_FPS();
void								HelpText_Render(HWND hwnd);


//  WindowsӦ�ó�����ں���
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{

	//һ�������Ĵ�����
	WNDCLASSEX wndClass = { 0 };				//��WINDCLASSEX������һ��������  
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;	//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;				//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra		= 0;
	wndClass.hInstance = hInstance;				//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );    //ָ��������Ĺ������
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = _T("GameDevelop");		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	if( !RegisterClassEx( &wndClass ) )				//�Դ��������ע�ᣬ���ܴ��������͵Ĵ���
		return -1;		

	hwnd = CreateWindow( _T("GameDevelop"),WINDOW_TITLE,			//�������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH,
		SCREEN_HEIGHT, NULL, NULL, hInstance, NULL );

	if (!(S_OK==Direct3D_Init (hwnd,hInstance)))
	{
		MessageBox(hwnd, _T("Direct3D��ʼ��ʧ��~��"), _T("��Ϣ����"), 0); //ʹ��MessageBox����������һ����Ϣ���� 
	}

	

	MoveWindow(hwnd,200,50,SCREEN_WIDTH,SCREEN_HEIGHT,true);   //����������ʾʱ��λ��
	ShowWindow( hwnd, nShowCmd );    //����Win32����ShowWindow����ʾ����
	UpdateWindow(hwnd);  //�Դ��ڽ��и���

	//����DirectInput��ĳ�ʼ��
	g_pDInput = new DInputClass();
	g_pDInput->Init(hwnd,hInstance,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//��Ϣѭ������
	MSG msg = { 0 };  //��ʼ��msg
	while( msg.message != WM_QUIT )			//ʹ��whileѭ��
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
		{
			TranslateMessage( &msg );		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage( &msg );		//�ú����ַ�һ����Ϣ�����ڳ���
		}
		else
		{
			Direct3D_Update(hwnd);         //���ø��º��������л���ĸ���
			Direct3D_Render(hwnd);			//������Ⱦ���������л������Ⱦ			
		}
	}

	UnregisterClass(_T("GameDevelop"), wndClass.hInstance);
	return 0;  
}

bool Login_judge()
{
	D3DXVECTOR3 pCenter;
	FLOAT pRadius = 0.0f;
	BYTE* vertices = NULL;
	g_box->LockVertexBuffer(0, (void**)&vertices);
	HRESULT hr = D3DXComputeBoundingSphere((D3DXVECTOR3*)vertices, g_box->GetNumVertices(),
		D3DXGetFVFVertexSize(g_box->GetFVF()), &pCenter, &pRadius);
	g_box->UnlockVertexBuffer();
	D3DXVECTOR3 viewPoint;
	g_pCamera->GetCameraPosition(&viewPoint);
	Box_pos -= viewPoint;
	if (D3DXVec3Length(&Box_pos) < 10 * pRadius)  //��Ϊ��½����������
		return true;
	return false;
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )   //���ڹ��̺���WndProc
{
	switch( message )				
	{
	case WM_PAINT:					 // �ͻ����ػ���Ϣ
		Direct3D_Render(hwnd);          //����Direct3D_Render���������л���Ļ���
		ValidateRect(hwnd, NULL);   // ���¿ͻ�������ʾ
		break;									//������switch���

	case WM_KEYDOWN:                // ���̰�����Ϣ
		if (wParam == VK_ESCAPE)    // ESC��
			DestroyWindow(hwnd);    // ���ٴ���, ������һ��WM_DESTROY��Ϣ
		else if (wParam == VK_SPACE)
		{
			if (LoginFlag == false) //���δ��¼
			{
				if (Login_judge())
				{
					Mess_notice->push_back(L"��¼�ɹ���");
					LoginFlag = true;
				}
				else
				{			
					Mess_notice->push_back(L"��¼ʧ�ܣ����ƶ����������壡");
				}
			}
			else
			{
				LoginFlag = false;
				Mess_notice->push_back(L"���뿪�������壡");
			}		
		}
		
		break;
	case WM_DESTROY:				//����������Ϣ
		Direct3D_CleanUp();     //����Direct3D_CleanUp����������COM�ӿڶ���
		PostQuitMessage( 0 );		//��ϵͳ�����и��߳�����ֹ����������ӦWM_DESTROY��Ϣ
		break;						//������switch���

	default:						//������case�����������ϣ���ִ�и�default���
		return DefWindowProc( hwnd, message, wParam, lParam );		//����ȱʡ�Ĵ��ڹ�����ΪӦ�ó���û�д���Ĵ�����Ϣ�ṩȱʡ�Ĵ���
	}

	return 0;					//�����˳�
}

HRESULT Direct3D_Init(HWND hwnd,HINSTANCE hInstance)
{

	LPDIRECT3D9  pD3D = NULL; //Direct3D�ӿڶ���Ĵ���
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) //��ʼ��Direct3D�ӿڶ��󣬲�����DirectX�汾Э��
 			return E_FAIL;

	D3DCAPS9 caps; int vp = 0;
	if( FAILED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ) )
		{
			return E_FAIL;
		}
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //֧��Ӳ���������㣬���ǾͲ���Ӳ���������㣬���׵�
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //��֧��Ӳ���������㣬����ֻ�ò��������������

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth            = SCREEN_WIDTH;
	d3dpp.BackBufferHeight           = SCREEN_HEIGHT;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 2;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;


	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	if(!(S_OK==Objects_Init())) return E_FAIL;

//	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	SAFE_RELEASE(pD3D) //LPDIRECT3D9�ӿڶ����ʹ����ɣ����ǽ����ͷŵ�

	return S_OK;
}



HRESULT Objects_Init()
{
	//��������
	D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS);
	D3DXCreateFont(g_pd3dDevice, 23, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"΢���ź�", &g_pTextHelper); 
	D3DXCreateFont(g_pd3dDevice, 26, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"����", &g_pTextInfor); 

	Box_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//
	Mess_notice = new MessNotice(g_pd3dDevice, 200, hwnd);

	// ����һƬ��ƺ
	g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, 
	D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pVertexBuffer, 0);

	CUSTOMVERTEX *pVertices = NULL;
	g_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-5000.0f, 0.0f, -5000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 100.0f);
	pVertices[1] = CUSTOMVERTEX(-5000.0f, 0.0f,  5000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX( 5000.0f, 0.0f, -5000.0f, 0.0f, 1.0f, 0.0f, 100.0f, 100.0f);
	pVertices[3] = CUSTOMVERTEX( 5000.0f, 0.0f,  5000.0f, 0.0f, 1.0f, 0.0f, 100.0f, 0.0f);
	g_pVertexBuffer->Unlock();

	// �����ذ�����
	D3DXCreateTextureFromFile(g_pd3dDevice, L"image/grass.jpg", &g_pTexture);
	// ����ѩ������
	D3DXCreateTextureFromFile(g_pd3dDevice, L"image/grass_2.jpg", &g_pTexture_snow);
	//������պ���
	g_pSkyBox = new CSkyBox(g_pd3dDevice);
	g_pSkyBox->InitSkyBox(10000.0f);
	g_pSkyBox->InitSkyBoxTexture(L"image/front.jpg", L"image/back.jpg", L"image/left.jpg", 
		L"image/right.jpg", L"image/top.jpg");


	//��������
	g_sphere = GenerateSphereMesh(g_pd3dDevice, 10.0f, 20, 20);

	//����ѩ�� 
	snow1 = new Snow_Mesh(g_pd3dDevice);
	snow2 = new Snow_Mesh(g_pd3dDevice);

	//����������
	g_box = GenerateBoxMesh(g_pd3dDevice, 60.0f, 60.0f, 60.0f);

	// ���÷������  
	//D3DLIGHT9 light;  
	//::ZeroMemory(&light, sizeof(light));  
	//light.Type          = D3DLIGHT_DIRECTIONAL;  
	//light.Ambient       = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);  
	//light.Diffuse       = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);  
	//light.Specular      = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);  
	//light.Direction     = D3DXVECTOR3(1.0f, 0.0f, 0.0f);  
	//g_pd3dDevice->SetLight(0, &light);  
	//g_pd3dDevice->LightEnable(0, true);  
	//g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);  
	//g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	////��������
	::ZeroMemory(&white, sizeof(white));
	white.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // 
	white.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // 
	white.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // 
	white.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); // no emission
	white.Power = 5.0f;

	//����һ����Դ
	D3DLIGHT9 light_point;
	::ZeroMemory(&light_point, sizeof(light_point));
	light_point.Type = D3DLIGHT_DIRECTIONAL;
	light_point.Ambient = D3DXCOLOR(0.9f, 0.9f, 0.9f, 0.0f);
	light_point.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	light_point.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.0f);
//	light_point.Position = D3DXVECTOR3(-1000.0f, 1000.0f, 2000.0f);
	light_point.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	g_pd3dDevice->SetLight(0, &light_point);
	g_pd3dDevice->LightEnable(0, true);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true); //�����������Ⱦ

	

	// ��������ʼ�����������
	g_pCamera = new CameraClass(g_pd3dDevice);
	g_pCamera->SetCameraPosition(&D3DXVECTOR3(0.0f, 0.0f, -1500.0f));  //������������ڵ�λ��
	g_pCamera->SetTargetPosition(&D3DXVECTOR3(0.0f, 500.0f, 300.0f));  //����Ŀ��۲�����ڵ�λ��
	g_pCamera->SetViewMatrix();  //����ȡ���任����
	g_pCamera->SetProjMatrix();  //����ͶӰ�任����

	

	// ����������˺�����Ѱַ��ʽ
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

void Direct3D_Update( HWND hwnd)
{
	//ʹ��DirectInput���ȡ����
	g_pDInput->GetInput();

	// ��������������ƶ��ӽ�
	if (g_pDInput->IsKeyDown(DIK_A))  g_pCamera->MoveAlongRightVec(-0.8f);
	if (g_pDInput->IsKeyDown(DIK_D))  g_pCamera->MoveAlongRightVec( 0.8f);
	if (g_pDInput->IsKeyDown(DIK_W)) g_pCamera->MoveAlongLookVec( 0.8f);
	if (g_pDInput->IsKeyDown(DIK_S))  g_pCamera->MoveAlongLookVec(-0.8f);
	if (g_pDInput->IsKeyDown(DIK_I))  g_pCamera->MoveAlongUpVec( 0.3f);
	if (g_pDInput->IsKeyDown(DIK_K))  g_pCamera->MoveAlongUpVec(-0.3f);

	//���������������ת�ӽ�
	if (g_pDInput->IsKeyDown(DIK_LEFT))  g_pCamera->RotationUpVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_RIGHT))  g_pCamera->RotationUpVec( 0.003f);
	if (g_pDInput->IsKeyDown(DIK_UP))  g_pCamera->RotationRightVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_DOWN))  g_pCamera->RotationRightVec( 0.003f);
	if (g_pDInput->IsKeyDown(DIK_J)) g_pCamera->RotationLookVec(0.001f);
	if (g_pDInput->IsKeyDown(DIK_L)) g_pCamera->RotationLookVec( -0.001f);

	//������������������������ת

	//���㲢����ȡ���任����
	if (LoginFlag == true)
	{
		g_pCamera->RotationVec(Box_pos, D3DXVECTOR3(0.0f, 1.0f, 0.0f), -D3DX_PI * 0.0002f);
	}
		D3DXMATRIX matView;
		g_pCamera->RotationUpVec(g_pDInput->MouseDX()* 0.001f);
		g_pCamera->RotationRightVec(g_pDInput->MouseDY() * 0.001f);
		g_pCamera->CalculateViewMatrix(&matView);
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

}
void Direct3D_Render(HWND hwnd)
{

	static int allcount = 0;
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(50, 100, 250), 1.0f, 0);

	g_pd3dDevice->BeginScene();                     // ��ʼ����
	//���Ƶƹ�
//	InitMaterialAndLight();

	g_pd3dDevice->SetMaterial(&white);



	
	//�������
	D3DXMatrixTranslation(&g_matWorld, 0.0f, -1500.0f, 0.0f);
	g_pSkyBox->RenderSkyBox(&g_matWorld);
	//������ʾ
	Mess_notice->Draw_mess();

	// ���Ʋ�ƺ
	D3DXMATRIX matWorld, RotMatrix;
	D3DXMatrixTranslation(&matWorld, 0.0f, -1000.0f, 0.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture);
	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//��������
	D3DXMatrixTranslation(&matWorld, 0.0f, 100.0f, -100.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture);
	g_sphere->DrawSubset(0);

	//����������

	D3DXMatrixTranslation(&matWorld, -1000.0f, -500.0f, -300.0f);
	D3DXMatrixRotationY(&RotMatrix, -D3DX_PI * 0.0002f* allcount);
	g_matWorld = matWorld * RotMatrix;
	Box_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&Box_pos, &Box_pos, &g_matWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture_snow);
	g_box->DrawSubset(0);

	//����ѩ��1
	D3DXMatrixTranslation(&matWorld, 0.0f, -500.0f, -100.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture_snow);
	snow1->Draw_Snow_body();

	//����ѩ��2
	D3DXMatrixTranslation(&matWorld, -1000.0f, -300.0f, -300.0f);
	D3DXMatrixRotationY(&RotMatrix, -D3DX_PI * 0.0002f* allcount);
	matWorld = matWorld * RotMatrix;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture_snow);
	snow1->Draw_Snow_body();
	   
	allcount = (allcount + 1) > 10000 ? 0 : (allcount + 1);
	HelpText_Render(hwnd);

	g_pd3dDevice->EndScene();                       // ��������

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // ��ʾ
	 
}


void HelpText_Render(HWND hwnd)
{
	//����һ�����Σ����ڻ�ȡ�����ھ���
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	// ���������Ϣ
	formatRect.left = 0,formatRect.top = 500;
	g_pTextInfor->DrawText(NULL, L"���ο���˵��:", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(235,123,230,255));
	formatRect.top += 35;
	g_pTextHelper->DrawText(NULL, L"    W����ǰ     S����� ", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    A������     D������", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    I����ֱ����     K����ֱ����", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    J��������б       L��������б", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    �ϡ��¡����ҷ����������ƶ����ӽǱ仯 ", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    ESC�� : �˳�����", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    Space�� : ��½���뿪������", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
}


void Direct3D_CleanUp()
{

	//�ͷ�COM�ӿڶ���
	SAFE_DELETE(g_pDInput);
	SAFE_RELEASE(g_pd3dDevice);
		SAFE_RELEASE(g_pTextHelper)
		SAFE_RELEASE(g_pTextInfor)
		SAFE_RELEASE(g_pTextFPS)
		SAFE_RELEASE(g_pd3dDevice)
	if (snow1)
	  delete snow1;
	if (snow2)
		delete snow2;
}