#pragma once

#include "WED3D.h"
#include "WEFullScreenQuad.h"

#define NUM_TONEMAP_TEXTURES 5

class DLL_EXPORT WEHDR
{
public:
    WEHDR();
    ~WEHDR();
    
    HRESULT Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
    void Release();

    void Process(WEFullScreenQuad* pFuffScreenQuad, ID3D11ShaderResourceView* pColorRV);
    
    HRESULT CreateViews();
    void ReleaseViews();

    ID3D11ShaderResourceView* GetLumRV()
    {
        return m_pTexToneMapRV[0];
    }
protected:
    ID3D11Device*               m_pd3dDevice;
    ID3D11DeviceContext*        m_pImmediateContext;
                                
    ID3D11PixelShader*          m_pPSDownScale2x2Lum;
    ID3D11PixelShader*          m_pPSDownScale3x3;
    ID3D11SamplerState*         m_pSamplePoint;
    
    ID3D11Texture2D*            m_pTexToneMap[NUM_TONEMAP_TEXTURES];
    ID3D11ShaderResourceView*   m_pTexToneMapRV[NUM_TONEMAP_TEXTURES];
    ID3D11RenderTargetView*     m_pTexToneMapRTV[NUM_TONEMAP_TEXTURES];
};

