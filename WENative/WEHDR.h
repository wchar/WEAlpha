#pragma once

#include "WED3D.h"
#include "WEFullScreenQuad.h"

#define NUM_TONEMAP_TEXTURES 5
#define NUM_BLOOM_TEXTURES 2

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

    ID3D11ShaderResourceView* GetBloomRV()
    {
        return m_pTexBloomRV[0];
    }

    ID3D11ShaderResourceView* GetBrightRV()
    {
        return m_pTexBrightPassRV;
    }

    ID3D11ShaderResourceView* GetFinalRV()
    {
        return m_pTexFinalRV;
    }

protected:
    ID3D11Device*               m_pd3dDevice;
    ID3D11DeviceContext*        m_pImmediateContext;
                                
    ID3D11SamplerState*         m_pSamplePoint;
    ID3D11SamplerState*         m_pSampleLinear;
    //
    void ProcessLum(ID3D11ShaderResourceView* pColorRV);    
    ID3D11PixelShader*          m_pPSDownScale2x2Lum;
    ID3D11PixelShader*          m_pPSDownScale3x3;
    ID3D11Texture2D*            m_pTexToneMap[NUM_TONEMAP_TEXTURES];
    ID3D11ShaderResourceView*   m_pTexToneMapRV[NUM_TONEMAP_TEXTURES];
    ID3D11RenderTargetView*     m_pTexToneMapRTV[NUM_TONEMAP_TEXTURES];
    
    //
    void ProcessBright(ID3D11ShaderResourceView* pColorRV);
    ID3D11PixelShader*          m_pPSBrightPass;
    ID3D11Texture2D*            m_pTexBrightPass;
    ID3D11ShaderResourceView*   m_pTexBrightPassRV;
    ID3D11RenderTargetView*     m_pTexBrightPassRTV;

    //
    void ProcessBloom();
    ID3D11Buffer*               m_pcbBloom;
    ID3D11PixelShader*          m_pPSBloom;
    ID3D11Texture2D*            m_pTexBloom[NUM_BLOOM_TEXTURES];
    ID3D11ShaderResourceView*   m_pTexBloomRV[NUM_BLOOM_TEXTURES];
    ID3D11RenderTargetView*     m_pTexBloomRTV[NUM_BLOOM_TEXTURES];

    //
    void ProcessFinalPass(ID3D11ShaderResourceView* pColorRV);
    ID3D11PixelShader*          m_pPSFinalPass;
    ID3D11Texture2D*            m_pTexFinal;
    ID3D11ShaderResourceView*   m_pTexFinalRV;
    ID3D11RenderTargetView*     m_pTexFinalRTV;

};

