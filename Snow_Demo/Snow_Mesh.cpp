#include "Snow_Mesh.h"
#include "GetMyMesh.h"



Snow_Mesh::Snow_Mesh(LPDIRECT3DDEVICE9 pDev)
{
	this->pDev = pDev;
	Head = GenerateSphereMesh(pDev, 20.0f, 100, 100);               //画出头部网格模型
	Body = GenerateCylinderMesh(pDev, 30.0f, 30.0f, 80.0f, 100, 100);//画出身体的网格模型
	LLeg = GenerateBoxMesh(pDev, 15.0f, 100.0f, 15.0f);  //画出腿的网格模型
	RLeg = GenerateBoxMesh(pDev, 15.0f, 100.0f, 15.0f);  //画出腿的网格模型
}


Snow_Mesh::~Snow_Mesh()
{
}
void Snow_Mesh::Draw_Snow_body()
{
	D3DXMATRIX matWorld, matTmp;
	D3DXMATRIX RotMatrix, FinalMatrix;
	D3DXMatrixRotationX(&RotMatrix, -D3DX_PI * 0.5f);
	pDev->GetTransform(D3DTS_WORLD, &matWorld);

	D3DXMatrixTranslation(&matTmp, 0.0f, -10.0f, 0.0f);
	FinalMatrix = (matWorld * matTmp);
	pDev->SetTransform(D3DTS_WORLD, &FinalMatrix);
	Head->DrawSubset(0);

	D3DXMatrixTranslation(&matTmp, 0.0f, -70.0f, 0.0f);
	FinalMatrix = (RotMatrix * matWorld * matTmp   );
	pDev->SetTransform(D3DTS_WORLD, &FinalMatrix);
	Body->DrawSubset(0);

	D3DXMatrixTranslation(&matTmp, 15.0f, -150.0f, 0.0f);
	FinalMatrix = matWorld * matTmp;
	pDev->SetTransform(D3DTS_WORLD, &FinalMatrix);
	LLeg->DrawSubset(0);

	D3DXMatrixTranslation(&matTmp, -15.0f, -150.0f, -0.0f);
	FinalMatrix = (matWorld * matTmp);
	pDev->SetTransform(D3DTS_WORLD, &FinalMatrix);
	RLeg->DrawSubset(0);
	

}
