#pragma once

#include "WEContent.h"
#include "WETexture.h"
#include "WEObjImporter.h"
#include "WEFbxImporter.h"

class DLL_EXPORT WEMaterialContent : public WEContent
{
public:
    friend class WEMaterial;
public:
    WEMaterialContent();
    ~WEMaterialContent();
    
    // override
    virtual UINT GetContentType()
    { 
        return eMaterial;
    }
    // read data form m_pStaticMemory
    virtual bool ToContentBuffer();
    // writer data on m_pStaticMemory
    virtual bool FromContentBuffer();

    bool Create(const MATERIAL* pMaterial);

    WEMaterial* CreateMaterial();


    bool        m_bEnableAlpha;
    bool        m_bEnableSpecular;

    FLOAT       m_fAlpha;
    FLOAT       m_fShininess;
    FLOAT       m_fStar;

    XMFLOAT3    m_vEmissive;
    XMFLOAT3    m_vAmbient;
    XMFLOAT3    m_vDiffuse;
    XMFLOAT3    m_vSpecular;

    WCHAR       m_strMaterialName[MAX_PATH];
protected:
    WETextureContent* m_pEmissiveTextureContent;
    WETextureContent* m_pDiffuseTextureContent;
    WETextureContent* m_pNormalTextureContent;
};
