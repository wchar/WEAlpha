#include "WEMaterial.h"


WEMaterial::WEMaterial(WEMaterialContent* pContent)
{
    m_pEmissiveTexture = NULL;
    m_pDiffuseTexture = NULL;
    m_pNormalTexture = NULL;

    if(pContent->m_pDiffuseTextureContent)
    {
        m_pDiffuseTexture = pContent->m_pDiffuseTextureContent->CreateTexture();
    }

    if(pContent->m_pEmissiveTextureContent)
    {
        m_pEmissiveTexture = pContent->m_pEmissiveTextureContent->CreateTexture();
    }

    if(pContent->m_pNormalTextureContent)
    {
        m_pNormalTexture = pContent->m_pNormalTextureContent->CreateTexture();
    }

    m_bEnableAlpha = pContent->m_bEnableAlpha;
    m_bEnableSpecular = pContent->m_bEnableSpecular;

    m_fStar = pContent->m_fStar;
    m_fShininess = pContent->m_fShininess;
    m_fAlpha = pContent->m_fAlpha;

    m_vAmbient = pContent->m_vAmbient;
    m_vDiffuse = pContent->m_vDiffuse;
    m_vEmissive = pContent->m_vEmissive;
    m_vSpecular = pContent->m_vSpecular;
}


WEMaterial::~WEMaterial()
{
    SAFE_DELETE(m_pEmissiveTexture);
    SAFE_DELETE(m_pDiffuseTexture);
    SAFE_DELETE(m_pNormalTexture);
}


int WEMaterial::GetMaterialFlag()
{
    int flag = 0;
    if (m_pDiffuseTexture)
        flag += 1;
    if (m_pEmissiveTexture)
        flag += 2;
    if (m_bEnableSpecular)
        flag += 4;

    return flag;
}