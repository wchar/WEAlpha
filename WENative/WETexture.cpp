#include "WETexture.h"

WETexture::WETexture(WETextureContent* pContent)
{
    HRESULT hr;
    V(D3DX11CreateShaderResourceViewFromMemory( WE::D3DDevice(), 
        pContent->m_pStaticMemory, pContent->m_uSize, NULL, NULL, &m_pSRV, NULL));
    if (FAILED(hr))
    {
        SAFE_RELEASE(m_pSRV);
    }
}

WETexture::~WETexture()
{
    SAFE_RELEASE(m_pSRV);
}
    