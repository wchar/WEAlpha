#pragma once

#include "WED3D.h"
#include "WEFullScreenQuad.h"

struct HDAOParams
{
    float fRejectRadius;
    float fAcceptRadius;
    float fIntensity;
    float fNormalScale;
    float fAcceptAngle;
};

class DLL_EXPORT WEHDAO
{
public:
    WEHDAO();
    ~WEHDAO();

    HRESULT Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
    void Release();

    HRESULT CreateViews();
    void ReleaseViews();

    void Process(WEFullScreenQuad* pFullScreenQuad, 
        ID3D11ShaderResourceView* pNormalSRV, 
        ID3D11ShaderResourceView* pDepthSRV); 


    ID3D11ShaderResourceView*   GetOcclusionRV()
    {
        return m_pTexOcclusionMapRV;
    }

    HDAOParams                  m_HDAOParams;

protected:
    ID3D11Device*               m_pd3dDevice;
    ID3D11DeviceContext*        m_pImmediateContext;

    ID3D11PixelShader*          m_pPixelShader;
    ID3D11SamplerState*         m_pSamplePoint;
    ID3D11Buffer*               m_pShaderBuffer;


    ID3D11Texture2D*            m_pTexOcclusionMap;
    ID3D11ShaderResourceView*   m_pTexOcclusionMapRV;
    ID3D11RenderTargetView*     m_pTexOcclusionMapRTV;
};

