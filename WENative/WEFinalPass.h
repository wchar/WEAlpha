#pragma once

#include "WED3D.h"
#include "WEFullScreenQuad.h"

class DLL_EXPORT WEFinalPass
{
public:
    WEFinalPass();
    ~WEFinalPass();

    HRESULT Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
    void Release();

    void Process(WEFullScreenQuad* pFullScreenQuad, 
        ID3D11RenderTargetView* pRTV,
        ID3D11ShaderResourceView* pColorRV,
        ID3D11ShaderResourceView* pDepthRV,
        ID3D11ShaderResourceView* pLightRV,
        ID3D11ShaderResourceView* pOcclusionRV,

        ID3D11ShaderResourceView* pStarRV,
        ID3D11ShaderResourceView* pBloomRV,
        ID3D11ShaderResourceView* pBlurRV,
        ID3D11ShaderResourceView* pFlogRV);

    void TestShaderResourceView(WEFullScreenQuad* pFullScreenQuad, 
        ID3D11RenderTargetView* pRTV, ID3D11ShaderResourceView* pSRV);
protected:
    ID3D11Device*               m_pd3dDevice;
    ID3D11DeviceContext*        m_pImmediateContext;

    ID3D11SamplerState*         m_pSamplePoint;
    ID3D11SamplerState*         m_pSampleLinear;

    ID3D11PixelShader*          m_pPSTestSRV;
    //ID3D11PixelShader*          m_pPSFinalPass1[2][2][2][2];
    //ID3D11PixelShader*          m_pPSFinalPass2[2][2][2][2];

};

