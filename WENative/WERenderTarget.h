#pragma once

#include "WED3D.h"

#define TARGET_COUNT 3

class WERenderTarget
{
public:
    WERenderTarget();
    ~WERenderTarget();

    HRESULT Create();
    void Release();

    void SetRenderTarget();
    void Clear();

    enum
    {
        eColor = 0,
        eNormal,
        eEmissive,
    };

    ID3D11ShaderResourceView* GetNormalSRV()
    {
        return m_pShaderResourceViewArray[eNormal];
    }
    ID3D11ShaderResourceView* GetColorSRV()
    {
        return m_pShaderResourceViewArray[eColor];
    }
    ID3D11ShaderResourceView* GetEmissiveSRV()
    {
        return m_pShaderResourceViewArray[eEmissive];
    }
protected:
    // RenderTarget including Normal, EmissiveColor and DiffuseColor.
    ID3D11Texture2D*            m_pRenderTargetTextureArray[TARGET_COUNT];
    ID3D11RenderTargetView*     m_pRenderTargetViewArray[TARGET_COUNT];
    ID3D11ShaderResourceView*   m_pShaderResourceViewArray[TARGET_COUNT];
    ID3D11Texture2D*            m_pDepthStencilBuffer;
    ID3D11DepthStencilView*     m_pDepthStencilView;
    D3D11_VIEWPORT              m_pViewport;
    ID3D11Buffer*               m_pScreenSpaceVertexBuffer;
};
