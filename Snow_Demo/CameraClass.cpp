
#include "CameraClass.h"

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH	800						//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define SCREEN_HEIGHT	600					//为窗口高度定义的宏，以方便在此处修改窗口高度
#endif

//-----------------------------------------------------------------------------
// Desc: 构造函数
//-----------------------------------------------------------------------------
CameraClass::CameraClass(IDirect3DDevice9 *pd3dDevice)
{
    m_pd3dDevice = pd3dDevice;
    m_vRightVector  = D3DXVECTOR3(1.0f, 0.0f, 0.0f);   // 默认右向量与X正半轴重合
    m_vUpVector     = D3DXVECTOR3(0.0f, 1.0f, 0.0f);   // 默认上向量与Y正半轴重合
    m_vLookVector   = D3DXVECTOR3(0.0f, 0.0f, 1.0f);   // 默认观察向量与Z正半轴重合
    m_vCameraPosition  = D3DXVECTOR3(0.0f, 0.0f, -250.0f);   // 默认摄像机坐标为(0.0f, 0.0f, -250.0f)
    m_vTargetPosition    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);//默认观察目标位置为(0.0f, 0.0f, 0.0f);

}


VOID CameraClass::CalculateViewMatrix(D3DXMATRIX *pMatrix) 
{
	//1.先把3个向量都规范化并使其相互垂直，成为一组正交矩阵
    D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);  //规范化观察分量
    D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);    // 上向量与观察向量垂直
    D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);                // 规范化上向量
    D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);    // 右向量与上向量垂直
    D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);          // 规范化右向量


    // 2.创建出取景变换矩阵
	//依次写出取景变换矩阵的第一行
    pMatrix->_11 = m_vRightVector.x;           // Rx
    pMatrix->_12 = m_vUpVector.x;              // Ux
    pMatrix->_13 = m_vLookVector.x;            // Lx
    pMatrix->_14 = 0.0f;
	//依次写出取景变换矩阵的第二行
    pMatrix->_21 = m_vRightVector.y;           // Ry
    pMatrix->_22 = m_vUpVector.y;              // Uy
    pMatrix->_23 = m_vLookVector.y;            // Ly
    pMatrix->_24 = 0.0f;
	//依次写出取景变换矩阵的第三行
    pMatrix->_31 = m_vRightVector.z;           // Rz
    pMatrix->_32 = m_vUpVector.z;              // Uz
    pMatrix->_33 = m_vLookVector.z;            // Lz
    pMatrix->_34 = 0.0f;
	//依次写出取景变换矩阵的第四行
    pMatrix->_41 = -D3DXVec3Dot(&m_vRightVector, &m_vCameraPosition);    // -P*R
    pMatrix->_42 = -D3DXVec3Dot(&m_vUpVector, &m_vCameraPosition);       // -P*U
    pMatrix->_43 = -D3DXVec3Dot(&m_vLookVector, &m_vCameraPosition);     // -P*L
    pMatrix->_44 = 1.0f;
}


//-----------------------------------------------------------------------------
// Name：CameraClass::SetTargetPosition( )
// Desc: 设置摄像机的观察位置
//-----------------------------------------------------------------------------
VOID CameraClass::SetTargetPosition(D3DXVECTOR3 *pLookat) 
{
	//先看看pLookat是否为默认值NULL
    if (pLookat != NULL)  m_vTargetPosition = (*pLookat);
    else m_vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    m_vLookVector = m_vTargetPosition - m_vCameraPosition;//观察点位置减摄像机位置，得到观察方向向量
    D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);//规范化m_vLookVector向量

	//正交并规范化m_vUpVector和m_vRightVector
    D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);
    D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);
    D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);
    D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);
}

VOID CameraClass::SetCameraPosition(D3DXVECTOR3 *pVector) 
{
    D3DXVECTOR3 V = D3DXVECTOR3(0.0f, 0.0f, -250.0f);
    m_vCameraPosition = pVector ? (*pVector) : V;//三目运算符，如果pVector为真的话，
																		//返回*pVector的值（即m_vCameraPosition=*pVector），
																		//否则返回V的值（即m_vCameraPosition=V）
}


VOID CameraClass::SetViewMatrix(D3DXMATRIX *pMatrix) 
{
	//根据pMatrix的值先做一下判断
    if (pMatrix) m_matView = *pMatrix;
    else CalculateViewMatrix(&m_matView);
    //把取景变换矩阵的值分下来分别给右分量，上分量，和观察分量
    m_vRightVector = D3DXVECTOR3(m_matView._11, m_matView._12, m_matView._13);
    m_vUpVector    = D3DXVECTOR3(m_matView._21, m_matView._22, m_matView._23);
    m_vLookVector  = D3DXVECTOR3(m_matView._31, m_matView._32, m_matView._33);
	CalcutePostion(m_matView);
}

VOID CameraClass::SetProjMatrix(D3DXMATRIX *pMatrix) 
{
	//判断值有没有，没有的话就计算一下
    if (pMatrix != NULL) m_matProj = *pMatrix;
    else D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4.0f, (float)((double)SCREEN_WIDTH/SCREEN_HEIGHT), 1.0f, 30000.0f);//视截体远景设为30000.0f，这样就不怕看不到远处的物体了
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);//设置投影变换矩阵
}


VOID CameraClass::MoveAlongRightVec(FLOAT fUnits) 
{
	//直接乘以fUnits的量来累加就行了
    m_vCameraPosition += m_vRightVector * fUnits;
    m_vTargetPosition   += m_vRightVector * fUnits;
}


VOID CameraClass::MoveAlongUpVec(FLOAT fUnits) 
{
	//直接乘以fUnits的量来累加就行了
    m_vCameraPosition += m_vUpVector * fUnits;
    m_vTargetPosition   += m_vUpVector * fUnits;
}

VOID CameraClass::MoveAlongLookVec(FLOAT fUnits) 
{
	//直接乘以fUnits的量来累加就行了
    m_vCameraPosition += m_vLookVector * fUnits;
    m_vTargetPosition   += m_vLookVector * fUnits;
}


VOID CameraClass::RotationRightVec(FLOAT fAngle) 
{
    D3DXMATRIX R;
    D3DXMatrixRotationAxis(&R, &m_vRightVector, fAngle);//创建出绕m_vRightVector旋转fAngle个角度的R矩阵
    D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);//让m_vCameraPosition向量绕m_vRightVector旋转fAngle个角度
    D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);//让m_vLookVector向量绕m_vRightVector旋转fAngle个角度

}


VOID CameraClass::RotationUpVec(FLOAT fAngle) 
{
    D3DXMATRIX R;
	D3DXVECTOR3  Tmp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXMatrixRotationAxis(&R, &Tmp, fAngle);//创建出绕m_vUpVector旋转fAngle个角度的R矩阵
    D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);//让m_vRightVector向量绕m_vUpVector旋转fAngle个角度
    D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);//让m_vLookVector向量绕m_vUpVector旋转fAngle个角度


}

VOID CameraClass::RotationLookVec(FLOAT fAngle) 
{
    D3DXMATRIX R;
    D3DXMatrixRotationAxis(&R, &m_vLookVector, fAngle);//创建出绕m_vLookVector旋转fAngle个角度的R矩阵
    D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);//让m_vRightVector向量绕m_vLookVector旋转fAngle个角度
    D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);//让m_vUpVector向量绕m_vLookVector旋转fAngle个角度

}

VOID CameraClass::RotationVec(D3DXVECTOR3 pos, D3DXVECTOR3 Vec, FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &Vec, fAngle);
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);
	D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
	//D3DXVec3TransformCoord(&m_vCameraPosition, &m_vCameraPosition, &R);
	m_vCameraPosition = pos + D3DXVECTOR3(0.0f, 200.0f, 0.0f);
}
VOID CameraClass::CalcutePostion(D3DXMATRIX pMatrix)
{
	D3DXMATRIX Res;
	D3DXMATRIX R = pMatrix;
	R._41 = 0.0f;
	R._42 = 0.0f;
	R._43 = 0.0f;
	D3DXMatrixInverse(&R, 0, &R);
	Res = pMatrix * R;
	m_vCameraPosition = D3DXVECTOR3(-Res._41, -Res._42, -Res._43);
}

CameraClass::~CameraClass(void)
{
}
