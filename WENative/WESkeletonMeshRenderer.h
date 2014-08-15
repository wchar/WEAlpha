#pragma once

#include "WED3D.h"
#include "WESkeletonMesh.h"
#include "WEMeshRenderer.h"

class DLL_EXPORT WESkeletonMeshRenderer : private WEMeshRenderer
{
public:
    WESkeletonMeshRenderer();
    ~WESkeletonMeshRenderer();

    HRESULT Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext);

    void DrawCascade(WESkeletonMesh* pMesh, 
        CXMMATRIX mWorld, 
        CXMMATRIX mView, 
        CXMMATRIX mProjection);

    void DrawScene(WESkeletonMesh* pMesh,
        CXMMATRIX mWorld, 
        CXMMATRIX mView, 
        CXMMATRIX mProjection, 
        CXMMATRIX mShadowView, 
        UINT uCascadeLevel);

private:
    void Release();
};

