#pragma once

#include "WEContent.h"
#include "WEMaterialContent.h"

class DLL_EXPORT WEMaterial
{
public:
    WEMaterial(WEMaterialContent* pContent);
    //WEMaterial(const WEMaterial& pArchetypeMaterial);
    ~WEMaterial();

    bool        m_bEnableAlpha;
    bool        m_bEnableSpecular;

    FLOAT       m_fAlpha;
    FLOAT       m_fShininess;
    FLOAT       m_fStar;

    XMFLOAT3    m_vEmissive;
    XMFLOAT3    m_vAmbient;
    XMFLOAT3    m_vDiffuse;
    XMFLOAT3    m_vSpecular;

    WETexture* GetEmissiveTexture()
    {
        return m_pEmissiveTexture;
    }
    WETexture* GetDiffuseTexture()
    {
        return m_pDiffuseTexture;
    }
    WETexture* GetNormalTexture()
    {
        return m_pNormalTexture;
    }
    int GetMaterialFlag();

protected:
    WETexture*      m_pEmissiveTexture;
    WETexture*      m_pDiffuseTexture;
    WETexture*      m_pNormalTexture;
};

