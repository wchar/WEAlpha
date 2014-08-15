#pragma once

#include "WEContent.h"
#include "WEMaterialContent.h"

class DLL_EXPORT WEMaterial
{
public:
    WEMaterial(WEMaterialContent* pContent);
    //WEMaterial(const WEMaterial& pArchetypeMaterial);
    ~WEMaterial();

    enum ShadingModel
    {
        ePhong,eLambert
    };

    ShadingModel    m_ShadingModel;
    XMFLOAT4        m_vEmissive;
    XMFLOAT4        m_vAmbient;
    XMFLOAT4        m_vDiffuse;
    XMFLOAT4        m_vSpecular;

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

protected:
    WETexture*      m_pEmissiveTexture;
    WETexture*      m_pDiffuseTexture;
    WETexture*      m_pNormalTexture;
};

