#pragma once

#include "WED3D.h"
#include "WEMesh.h"
#include "WESkeletonMesh.h"
#include "WECascadeShadowMisc.h"

class DLL_EXPORT WEMeshRenderer
{
public:
    WEMeshRenderer();
    ~WEMeshRenderer();  

    HRESULT Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext);

    void DrawCascade(WEMesh* pMesh, 
        CXMMATRIX mWorld, 
        CXMMATRIX mView, 
        CXMMATRIX mProjection);

    void DrawScene(WEMesh* pMesh, 
        CXMMATRIX mWorld, 
        CXMMATRIX mView, 
        CXMMATRIX mProjection, 
        CXMMATRIX mShadowView, 
        UINT uCascadeLevel);

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

protected:
    ID3D11Device*           m_pd3dDevice;
    ID3D11DeviceContext*    m_pImmediateContext;

    void Release();

    ID3D11Buffer*           m_pVSBuffer;
    ID3D11Buffer*           m_pPSBuffer;

    ID3D11InputLayout*      m_pVertexLayoutMesh;
    ID3D11InputLayout*      m_pVertexLayoutSkeletonMesh;

    ID3D11VertexShader*     m_pvsRenderVarianceShadow;
    ID3DBlob*               m_pvsRenderVarianceShadowBlob;
    ID3D11PixelShader*      m_ppsRenderVarianceShadow;
    ID3DBlob*               m_ppsRenderVarianceShadowBlob;

    ID3D11VertexShader*     m_pvsRenderScene;
    ID3DBlob*               m_pvsRenderSceneBlob;

    ID3D11VertexShader*     m_pvsRenderSkeletonVarianceShadow;
    ID3DBlob*               m_pvsRenderSkeletonVarianceShadowBlob;
    ID3D11VertexShader*     m_pvsRenderSkeletonScene;
    ID3DBlob*               m_pvsRenderSkeletonSceneBlob;
    
    //BLEND_BETWEEN_CASCADE_LAYERS_FLAG
    //SELECT_CASCADE_BY_INTERVAL_FLAG
    ID3D11PixelShader*      m_ppsRenderSceneAllShaders[MAX_CASCADES][2][2];
    ID3DBlob*               m_ppsRenderSceneAllShadersBlob[MAX_CASCADES][2][2];
};

