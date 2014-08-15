#pragma once

#include "WED3D.h"
#include "WEFullScreenQuad.h"

class WEDeferredLighting
{
public:
    WEDeferredLighting();
    ~WEDeferredLighting();

    HRESULT Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
    void Release();

    void Process(WEFullScreenQuad* pFullScreenQuad, ID3D11ShaderResourceView* pColorSRV, ID3D11ShaderResourceView* pNormalSRV, XMFLOAT3 vLightDir); 

    HRESULT CreateViews();
    void ReleaseViews();

    ID3D11ShaderResourceView* GetLightRV()
    {
        return m_pTexLightMapRV;
    }
protected:
    ID3D11Device*               m_pd3dDevice;
    ID3D11DeviceContext*        m_pImmediateContext;

    ID3D11PixelShader*          m_pPixelShader;
    ID3D11SamplerState*         m_pSamplePoint;
    ID3D11Buffer*               m_pShaderBuffer;

    ID3D11Texture2D*            m_pTexLightMap;
    ID3D11ShaderResourceView*   m_pTexLightMapRV;
    ID3D11RenderTargetView*     m_pTexLightMapRTV;
};

 