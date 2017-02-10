#include "GetMyMesh.h"
struct VERTEX
{
	D3DXVECTOR3 pos;    // Vertex position
	D3DXVECTOR3 norm;    // Vertex normal
	float tu;            // Texture coordinate u
	float tv;            // Texture coordinate v
};


LPD3DXMESH GenerateBoxMesh(LPDIRECT3DDEVICE9 pDev, float a, float b, float c)
{
	// Create D3D sphere
	LPD3DXMESH mesh;
	if (FAILED(D3DXCreateBox(pDev, a, b, c, &mesh, NULL)))
	{
		return NULL;
	}

	// Get a copy of the sphere mesh
	LPD3DXMESH texMesh;
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, FVF_VERTEX, pDev, &texMesh)))
	{
		return NULL;
	}

	// Release original mesh
	mesh->Release();

	// add texture coordinates
	VERTEX* pVerts;
	if (SUCCEEDED(texMesh->LockVertexBuffer(0, (void**)&pVerts)))
	{
		// Get vertex count
		int numVerts = texMesh->GetNumVertices();

		for (int i = 0; i < numVerts; ++i)
		{
			D3DXVECTOR3 v = pVerts->pos;
			D3DXVec3Normalize(&v, &v);

			// Calculates texture coordinates
			pVerts->tu = asinf(v.x) / D3DX_PI + 0.5f;
			pVerts->tv = asinf(v.y) / D3DX_PI + 0.5f;

			++pVerts;
		}

		// Unlock the vertex buffer
		texMesh->UnlockVertexBuffer();
	}

	return texMesh;
}

LPD3DXMESH GenerateSphereMesh(LPDIRECT3DDEVICE9 pDev,
	float fRadius, UINT slices, UINT stacks)
{
	// Create D3D sphere
	LPD3DXMESH mesh;
	if (FAILED(D3DXCreateSphere(pDev, fRadius, slices, stacks, &mesh, NULL)))
	{
		return NULL;
	}

	// Get a copy of the sphere mesh
	LPD3DXMESH texMesh;
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, FVF_VERTEX, pDev, &texMesh)))
	{
		return NULL;
	}

	// Release original mesh
	mesh->Release();

	// add texture coordinates
	VERTEX* pVerts;
	if (SUCCEEDED(texMesh->LockVertexBuffer(0, (void**)&pVerts)))
	{
		// Get vertex count
		int numVerts = texMesh->GetNumVertices();

		for (int i = 0; i < numVerts; ++i)
		{
			// Calculates texture coordinates
			pVerts->tu = asinf(pVerts->norm.x) / D3DX_PI + 0.5f;
			pVerts->tv = asinf(pVerts->norm.y) / D3DX_PI + 0.5f;
			++pVerts;
		}

		// Unlock the vertex buffer
		texMesh->UnlockVertexBuffer();
	}

	return texMesh;
}

LPD3DXMESH GenerateCylinderMesh(LPDIRECT3DDEVICE9 pDev, float Radius1, float Radius2, float high, UINT slices, UINT stacks)
{
	LPD3DXMESH mesh;
	if (FAILED(D3DXCreateCylinder(pDev, Radius1, Radius2, high, slices, stacks, &mesh, NULL)))
	{
		return NULL;
	}

	// Get a copy of the sphere mesh
	LPD3DXMESH texMesh;
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, FVF_VERTEX, pDev, &texMesh)))
	{
		return NULL;
	}

	// Release original mesh
	mesh->Release();

	// add texture coordinates
	VERTEX* pVerts;
	if (SUCCEEDED(texMesh->LockVertexBuffer(0, (void**)&pVerts)))
	{
		// Get vertex count
		int numVerts = texMesh->GetNumVertices();

		for (int i = 0; i < numVerts; ++i)
		{
			// Calculates texture coordinates
			pVerts->tu = asinf(pVerts->norm.x) / D3DX_PI + 0.5f;
			pVerts->tv = asinf(pVerts->norm.y) / D3DX_PI + 0.5f;
			++pVerts;
		}

		// Unlock the vertex buffer
		texMesh->UnlockVertexBuffer();
	}

	return texMesh;
}