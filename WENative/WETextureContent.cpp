#include "WETextureContent.h"
#include "WETexture.h"


WETextureContent::WETextureContent()
{
    m_pTexture = NULL;
};

WETextureContent::~WETextureContent()
{
    SAFE_RELEASE(m_pTexture);
}

bool WETextureContent::ToContentBuffer()
{
    if (m_pTexture)
    {
        // create Blob
        ID3DBlob* pBlob;
        if (FAILED(D3DX11SaveTextureToMemory( WE::ImmediateContext(), 
            m_pTexture, D3DX11_IFF_DDS, &pBlob, NULL )))
        {
            SAFE_RELEASE(pBlob);
            return false;
        }

        // copy to the m_pStaticMemory
        m_uSize = pBlob->GetBufferSize();
        m_pStaticMemory = new BYTE[m_uSize];
        CopyMemory(m_pStaticMemory, pBlob->GetBufferPointer(), m_uSize);

        SAFE_RELEASE(pBlob);    
        
        return true;
    }

    return false;
}

bool WETextureContent::FromContentBuffer()
{
    if (!m_pStaticMemory)
        return false;

    // try to create resource from m_pStaticMemory
    HRESULT hr;
    V(D3DX11CreateTextureFromMemory( WE::D3DDevice(), 
        m_pStaticMemory, m_uSize, NULL, NULL, &m_pTexture, NULL));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool WETextureContent::Import(const WCHAR* wstrFile)
{
    // create resource from file
    HRESULT hr;
    V( D3DX11CreateTextureFromFile( WE::D3DDevice(),
        wstrFile, NULL, NULL, &m_pTexture, NULL ) );
    if (FAILED(hr))
    {
        return false;
    }

    // create memory buffer
    return ToContentBuffer();
}

WETexture* WETextureContent::CreateTexture()
{
    return new WETexture(this);
}
