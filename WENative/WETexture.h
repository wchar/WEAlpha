#pragma once

#include "WETextureContent.h"

class DLL_EXPORT WETexture
{
public:
    WETexture(WETextureContent* pContent);
    ~WETexture();

    ID3D11ShaderResourceView* GetSRV()
    {
        return m_pSRV;
    }

    operator ID3D11ShaderResourceView*() const  
    {
        return m_pSRV;
    }
protected:
    ID3D11ShaderResourceView* m_pSRV;
};

