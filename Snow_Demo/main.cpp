
#define SCREEN_WIDTH	1440						//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define SCREEN_HEIGHT	760							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	_T("简易雪人场景实现") //为窗口标题定义的宏
                                                                                      
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
#pragma comment(lib, "dinput8.lib")     // 使用DirectInput必须包含的库文件，注意这里有8
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "winmm.lib") 


//定义顶点结构体
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
// Desc: 全局变量声明部分  
//*****************************************************************************************
LPDIRECT3DDEVICE9					g_pd3dDevice = NULL; //Direct3D设备对象
LPD3DXFONT								g_pTextFPS				=NULL;    //字体COM接口
LPD3DXFONT								g_pTextHelper          = NULL;  // 帮助信息的2D文本
LPD3DXFONT								g_pTextInfor           = NULL;  // 绘制信息的2D文本
float								g_FPS		= 0.0f;       //一个浮点型的变量，代表帧速率
wchar_t										g_strFPS[50]={0};    //包含帧速率的字符数组
D3DXMATRIX							g_matWorld;   //世界矩阵
DInputClass*								g_pDInput = NULL;         //一个DInputClass类的指针
CameraClass*							g_pCamera  = NULL;
//
LPD3DXMESH							g_sphere = NULL;     //球网格对象
LPD3DXMESH							g_box = NULL;     //盒子网格对象
//
LPDIRECT3DVERTEXBUFFER9		g_pVertexBuffer           = NULL;  //绘制草地的顶点缓存对象
LPDIRECT3DTEXTURE9				g_pTexture             = NULL;  //绘制草地的纹理对象
LPDIRECT3DTEXTURE9				g_pTexture_snow = NULL;  //绘制雪人的纹理对象
D3DMATERIAL9					white;
HWND hwnd = NULL;
Snow_Mesh*  snow1 = NULL;
Snow_Mesh*  snow2 = NULL;
CSkyBox*      g_pSkyBox = NULL;    //天空盒类指针  
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


//  Windows应用程序入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{

	//一个完整的窗口类
	WNDCLASSEX wndClass = { 0 };				//用WINDCLASSEX定义了一个窗口类  
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;	//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;				//设置指向窗口过程函数的指针
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra		= 0;
	wndClass.hInstance = hInstance;				//指定包含窗口过程的程序的实例句柄。
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );    //指定窗口类的光标句柄。
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = _T("GameDevelop");		//用一个以空终止的字符串，指定窗口类的名字。

	if( !RegisterClassEx( &wndClass ) )				//对窗口类进行注册，才能创建该类型的窗口
		return -1;		

	hwnd = CreateWindow( _T("GameDevelop"),WINDOW_TITLE,			//创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH,
		SCREEN_HEIGHT, NULL, NULL, hInstance, NULL );

	if (!(S_OK==Direct3D_Init (hwnd,hInstance)))
	{
		MessageBox(hwnd, _T("Direct3D初始化失败~！"), _T("消息窗口"), 0); //使用MessageBox函数，创建一个消息窗口 
	}

	

	MoveWindow(hwnd,200,50,SCREEN_WIDTH,SCREEN_HEIGHT,true);   //调整窗口显示时的位置
	ShowWindow( hwnd, nShowCmd );    //调用Win32函数ShowWindow来显示窗口
	UpdateWindow(hwnd);  //对窗口进行更新

	//进行DirectInput类的初始化
	g_pDInput = new DInputClass();
	g_pDInput->Init(hwnd,hInstance,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//消息循环过程
	MSG msg = { 0 };  //初始化msg
	while( msg.message != WM_QUIT )			//使用while循环
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage( &msg );		//将虚拟键消息转换为字符消息
			DispatchMessage( &msg );		//该函数分发一个消息给窗口程序。
		}
		else
		{
			Direct3D_Update(hwnd);         //调用更新函数，进行画面的更新
			Direct3D_Render(hwnd);			//调用渲染函数，进行画面的渲染			
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
	if (D3DXVec3Length(&Box_pos) < 10 * pRadius)  //认为登陆到了立方体
		return true;
	return false;
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )   //窗口过程函数WndProc
{
	switch( message )				
	{
	case WM_PAINT:					 // 客户区重绘消息
		Direct3D_Render(hwnd);          //调用Direct3D_Render函数，进行画面的绘制
		ValidateRect(hwnd, NULL);   // 更新客户区的显示
		break;									//跳出该switch语句

	case WM_KEYDOWN:                // 键盘按下消息
		if (wParam == VK_ESCAPE)    // ESC键
			DestroyWindow(hwnd);    // 销毁窗口, 并发送一条WM_DESTROY消息
		else if (wParam == VK_SPACE)
		{
			if (LoginFlag == false) //如果未登录
			{
				if (Login_judge())
				{
					Mess_notice->push_back(L"登录成功！");
					LoginFlag = true;
				}
				else
				{			
					Mess_notice->push_back(L"登录失败，请移动靠近立方体！");
				}
			}
			else
			{
				LoginFlag = false;
				Mess_notice->push_back(L"你离开了立方体！");
			}		
		}
		
		break;
	case WM_DESTROY:				//窗口销毁消息
		Direct3D_CleanUp();     //调用Direct3D_CleanUp函数，清理COM接口对象
		PostQuitMessage( 0 );		//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
		break;						//跳出该switch语句

	default:						//若上述case条件都不符合，则执行该default语句
		return DefWindowProc( hwnd, message, wParam, lParam );		//调用缺省的窗口过程来为应用程序没有处理的窗口消息提供缺省的处理。
	}

	return 0;					//正常退出
}

HRESULT Direct3D_Init(HWND hwnd,HINSTANCE hInstance)
{

	LPDIRECT3D9  pD3D = NULL; //Direct3D接口对象的创建
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) //初始化Direct3D接口对象，并进行DirectX版本协商
 			return E_FAIL;

	D3DCAPS9 caps; int vp = 0;
	if( FAILED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ) )
		{
			return E_FAIL;
		}
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //支持硬件顶点运算，我们就采用硬件顶点运算，妥妥的
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //不支持硬件顶点运算，无奈只好采用软件顶点运算

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

	SAFE_RELEASE(pD3D) //LPDIRECT3D9接口对象的使命完成，我们将其释放掉

	return S_OK;
}



HRESULT Objects_Init()
{
	//创建字体
	D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS);
	D3DXCreateFont(g_pd3dDevice, 23, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"微软雅黑", &g_pTextHelper); 
	D3DXCreateFont(g_pd3dDevice, 26, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"黑体", &g_pTextInfor); 

	Box_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//
	Mess_notice = new MessNotice(g_pd3dDevice, 200, hwnd);

	// 创建一片草坪
	g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, 
	D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pVertexBuffer, 0);

	CUSTOMVERTEX *pVertices = NULL;
	g_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-5000.0f, 0.0f, -5000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 100.0f);
	pVertices[1] = CUSTOMVERTEX(-5000.0f, 0.0f,  5000.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX( 5000.0f, 0.0f, -5000.0f, 0.0f, 1.0f, 0.0f, 100.0f, 100.0f);
	pVertices[3] = CUSTOMVERTEX( 5000.0f, 0.0f,  5000.0f, 0.0f, 1.0f, 0.0f, 100.0f, 0.0f);
	g_pVertexBuffer->Unlock();

	// 创建地板纹理
	D3DXCreateTextureFromFile(g_pd3dDevice, L"image/grass.jpg", &g_pTexture);
	// 创建雪人纹理
	D3DXCreateTextureFromFile(g_pd3dDevice, L"image/grass_2.jpg", &g_pTexture_snow);
	//创建天空盒子
	g_pSkyBox = new CSkyBox(g_pd3dDevice);
	g_pSkyBox->InitSkyBox(10000.0f);
	g_pSkyBox->InitSkyBoxTexture(L"image/front.jpg", L"image/back.jpg", L"image/left.jpg", 
		L"image/right.jpg", L"image/top.jpg");


	//创建球体
	g_sphere = GenerateSphereMesh(g_pd3dDevice, 10.0f, 20, 20);

	//创建雪人 
	snow1 = new Snow_Mesh(g_pd3dDevice);
	snow2 = new Snow_Mesh(g_pd3dDevice);

	//创建立方体
	g_box = GenerateBoxMesh(g_pd3dDevice, 60.0f, 60.0f, 60.0f);

	// 设置方向光照  
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

	////创建材质
	::ZeroMemory(&white, sizeof(white));
	white.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // 
	white.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // 
	white.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // 
	white.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); // no emission
	white.Power = 5.0f;

	//设置一个光源
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
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true); //开启镜面光渲染

	

	// 创建并初始化虚拟摄像机
	g_pCamera = new CameraClass(g_pd3dDevice);
	g_pCamera->SetCameraPosition(&D3DXVECTOR3(0.0f, 0.0f, -1500.0f));  //设置摄像机所在的位置
	g_pCamera->SetTargetPosition(&D3DXVECTOR3(0.0f, 500.0f, 300.0f));  //设置目标观察点所在的位置
	g_pCamera->SetViewMatrix();  //设置取景变换矩阵
	g_pCamera->SetProjMatrix();  //设置投影变换矩阵

	

	// 设置纹理过滤和纹理寻址方式
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

void Direct3D_Update( HWND hwnd)
{
	//使用DirectInput类读取数据
	g_pDInput->GetInput();

	// 沿摄像机各分量移动视角
	if (g_pDInput->IsKeyDown(DIK_A))  g_pCamera->MoveAlongRightVec(-0.8f);
	if (g_pDInput->IsKeyDown(DIK_D))  g_pCamera->MoveAlongRightVec( 0.8f);
	if (g_pDInput->IsKeyDown(DIK_W)) g_pCamera->MoveAlongLookVec( 0.8f);
	if (g_pDInput->IsKeyDown(DIK_S))  g_pCamera->MoveAlongLookVec(-0.8f);
	if (g_pDInput->IsKeyDown(DIK_I))  g_pCamera->MoveAlongUpVec( 0.3f);
	if (g_pDInput->IsKeyDown(DIK_K))  g_pCamera->MoveAlongUpVec(-0.3f);

	//沿摄像机各分量旋转视角
	if (g_pDInput->IsKeyDown(DIK_LEFT))  g_pCamera->RotationUpVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_RIGHT))  g_pCamera->RotationUpVec( 0.003f);
	if (g_pDInput->IsKeyDown(DIK_UP))  g_pCamera->RotationRightVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_DOWN))  g_pCamera->RotationRightVec( 0.003f);
	if (g_pDInput->IsKeyDown(DIK_J)) g_pCamera->RotationLookVec(0.001f);
	if (g_pDInput->IsKeyDown(DIK_L)) g_pCamera->RotationLookVec( -0.001f);

	//鼠标控制右向量和上向量的旋转

	//计算并设置取景变换矩阵
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

	g_pd3dDevice->BeginScene();                     // 开始绘制
	//绘制灯光
//	InitMaterialAndLight();

	g_pd3dDevice->SetMaterial(&white);



	
	//绘制天空
	D3DXMatrixTranslation(&g_matWorld, 0.0f, -1500.0f, 0.0f);
	g_pSkyBox->RenderSkyBox(&g_matWorld);
	//绘制提示
	Mess_notice->Draw_mess();

	// 绘制草坪
	D3DXMATRIX matWorld, RotMatrix;
	D3DXMatrixTranslation(&matWorld, 0.0f, -1000.0f, 0.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture);
	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//绘制球体
	D3DXMatrixTranslation(&matWorld, 0.0f, 100.0f, -100.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture);
	g_sphere->DrawSubset(0);

	//绘制立方体

	D3DXMatrixTranslation(&matWorld, -1000.0f, -500.0f, -300.0f);
	D3DXMatrixRotationY(&RotMatrix, -D3DX_PI * 0.0002f* allcount);
	g_matWorld = matWorld * RotMatrix;
	Box_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&Box_pos, &Box_pos, &g_matWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture_snow);
	g_box->DrawSubset(0);

	//绘制雪人1
	D3DXMatrixTranslation(&matWorld, 0.0f, -500.0f, -100.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture_snow);
	snow1->Draw_Snow_body();

	//绘制雪人2
	D3DXMatrixTranslation(&matWorld, -1000.0f, -300.0f, -300.0f);
	D3DXMatrixRotationY(&RotMatrix, -D3DX_PI * 0.0002f* allcount);
	matWorld = matWorld * RotMatrix;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetTexture(0, g_pTexture_snow);
	snow1->Draw_Snow_body();
	   
	allcount = (allcount + 1) > 10000 ? 0 : (allcount + 1);
	HelpText_Render(hwnd);

	g_pd3dDevice->EndScene();                       // 结束绘制

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // 显示
	 
}


void HelpText_Render(HWND hwnd)
{
	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	// 输出帮助信息
	formatRect.left = 0,formatRect.top = 500;
	g_pTextInfor->DrawText(NULL, L"漫游控制说明:", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(235,123,230,255));
	formatRect.top += 35;
	g_pTextHelper->DrawText(NULL, L"    W：向前     S：向后 ", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    A：向左     D：向右", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    I：垂直向上     K：垂直向下", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    J：向左倾斜       L：向右倾斜", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    上、下、左、右方向键、鼠标移动：视角变化 ", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    ESC键 : 退出程序", -1, &formatRect, 
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
	formatRect.top += 25;
	g_pTextHelper->DrawText(NULL, L"    Space键 : 登陆或离开立方体", -1, &formatRect,
		DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255, 200, 0, 255));
}


void Direct3D_CleanUp()
{

	//释放COM接口对象
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