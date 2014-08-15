#include "WEMaterialContent.h"
#include "WEMaterial.h"

namespace
{
    struct Head
    {
        bool bEnableAlpha;
        bool bEnableSpecular;

        XMFLOAT4 vEmissive;
        XMFLOAT4 vAmbient;
        XMFLOAT4 vDiffuse;
        XMFLOAT4 vSpecular;

        UINT uOffsetTexDiffuse;
        UINT uOffsetTexEmissive;
        UINT uOffsetTexNormal;
    };
}

WEMaterialContent::WEMaterialContent(void)
{
    m_pNormalTextureContent = NULL;
    m_pDiffuseTextureContent = NULL;
    m_pEmissiveTextureContent = NULL;                     
}


WEMaterialContent::~WEMaterialContent(void)
{
    SAFE_DELETE(m_pNormalTextureContent);
    SAFE_DELETE(m_pDiffuseTextureContent);
    SAFE_DELETE(m_pEmissiveTextureContent);
}

bool WEMaterialContent::Create(WEObjImporter::Material* pObjMaterial)
{
    m_bEnableSpecular = pObjMaterial->bSpecular;
    m_bEnableAlpha = pObjMaterial->fAlpha >= 1.0f;

    pObjMaterial->nShininess;

    if (pObjMaterial->bTexture)
    {
        m_pDiffuseTextureContent = new WETextureContent();
        m_pDiffuseTextureContent->Import(pObjMaterial->strTexture);
    }
    CopyMemory(&m_vAmbient,&pObjMaterial->vAmbient, sizeof(XMFLOAT3));
    CopyMemory(&m_vSpecular, &pObjMaterial->vSpecular, sizeof(XMFLOAT3));
    CopyMemory(&m_vDiffuse, &pObjMaterial->vDiffuse, sizeof(XMFLOAT3));

    m_vDiffuse.w = pObjMaterial->fAlpha;
    m_vAmbient.w = 1.0f;
    m_vSpecular.w = 1.0f;
 
    if (ToContentBuffer())
        return true;

    SAFE_DELETE(m_pDiffuseTextureContent);
    return false;
}

bool WEMaterialContent::Create(const MATERIAL* pMaterial)
{
    m_bEnableSpecular = pMaterial->bSpecular;
    m_bEnableAlpha = pMaterial->fOpacity >= 1.0f;

    pMaterial->fShinness;

    if (pMaterial->bDiffuseTexture)
    {
        m_pDiffuseTextureContent = new WETextureContent();
        m_pDiffuseTextureContent->Import(pMaterial->strDiffuseTexture);
    }
    if (pMaterial->bEmissiveTexture)
    {
        m_pEmissiveTextureContent = new WETextureContent();
        m_pEmissiveTextureContent->Import(pMaterial->strEmissiveTexture);
    }
    if (pMaterial->bNormalMap)
    {
        m_pNormalTextureContent = new WETextureContent();
        m_pNormalTextureContent->Import(pMaterial->strNormalMap);
    }

    CopyMemory(&m_vAmbient,&pMaterial->vAmbient, sizeof(XMFLOAT3));
    CopyMemory(&m_vSpecular, &pMaterial->vSpecular, sizeof(XMFLOAT3));
    CopyMemory(&m_vDiffuse, &pMaterial->vDiffuse, sizeof(XMFLOAT3));

    m_vDiffuse.w = pMaterial->fOpacity;
    m_vAmbient.w = 1.0f;
    m_vSpecular.w = 1.0f;

    if (ToContentBuffer())
        return true;

    SAFE_DELETE(m_pDiffuseTextureContent);
    return false;
}

WEMaterial* WEMaterialContent::CreateMaterial()
{
    return new WEMaterial(this);
}

bool WEMaterialContent::ToContentBuffer()
{
    SAFE_DELETE(m_pStaticMemory);

    Head head;
    // Filling head structure.
    head.bEnableAlpha = m_bEnableAlpha;
    head.bEnableSpecular = m_bEnableSpecular;
    head.vDiffuse = m_vDiffuse;
    head.vAmbient = m_vAmbient;
    head.vEmissive = m_vEmissive;
    head.vSpecular = m_vSpecular;

    head.uOffsetTexDiffuse = sizeof(Head);
    if (m_pDiffuseTextureContent)
        head.uOffsetTexEmissive = head.uOffsetTexDiffuse + m_pDiffuseTextureContent->GetSize();
    else
        head.uOffsetTexEmissive = head.uOffsetTexDiffuse;
    if (m_pEmissiveTextureContent)
        head.uOffsetTexNormal = head.uOffsetTexEmissive + m_pEmissiveTextureContent->GetSize();
    else
        head.uOffsetTexNormal = head.uOffsetTexEmissive;

    // Calculating size of the buffer.
    if (m_pNormalTextureContent)
        m_uSize = head.uOffsetTexNormal + m_pNormalTextureContent->GetSize();
    else
        m_uSize = head.uOffsetTexNormal;

    // Allocating memory and fill it
    m_pStaticMemory = new BYTE[m_uSize];
    BYTE *pBuf = m_pStaticMemory;
    CopyMemory(pBuf, &head, sizeof(head));
    pBuf += sizeof(head);
    if (m_pDiffuseTextureContent)
    {
        CopyMemory(pBuf, m_pDiffuseTextureContent->GetMemoryPtr(), m_pDiffuseTextureContent->GetSize());
        pBuf += m_pDiffuseTextureContent->GetSize();
    }
    if (m_pEmissiveTextureContent)
    {
        CopyMemory(pBuf, m_pEmissiveTextureContent->GetMemoryPtr(), m_pEmissiveTextureContent->GetSize());
        pBuf += m_pEmissiveTextureContent->GetSize();
    }
    if (m_pNormalTextureContent)
    {
        CopyMemory(pBuf, m_pNormalTextureContent->GetMemoryPtr(), m_pNormalTextureContent->GetSize());
        pBuf += m_pNormalTextureContent->GetSize();
    }
    if ((pBuf - m_pStaticMemory) == m_uSize)
        return true;

    SAFE_DELETE(m_pStaticMemory);
    m_uSize = 0;

    return false;
}

bool WEMaterialContent::FromContentBuffer()
{
    return false;
}