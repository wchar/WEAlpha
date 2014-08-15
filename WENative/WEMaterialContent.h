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

    bool Create(WEObjImporter::Material* pObjMaterial);
    bool Create(const MATERIAL* pMaterial);

    WEMaterial* CreateMaterial();
protected:
    bool    m_bEnableAlpha;
    bool    m_bEnableSpecular;

    XMFLOAT4    m_vEmissive;
    XMFLOAT4    m_vAmbient;
    XMFLOAT4    m_vDiffuse;
    XMFLOAT4    m_vSpecular;

    WETextureContent* m_pEmissiveTextureContent;
    WETextureContent* m_pDiffuseTextureContent;
    WETextureContent* m_pNormalTextureContent;
};
