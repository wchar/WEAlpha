#include "WESkeletonMeshContent.h"
#include "WESkeletonMesh.h"


//-----------------------------------
WESkeletonMeshContent::WESkeletonMeshContent()
{
}


WESkeletonMeshContent::~WESkeletonMeshContent()
{
}


bool WESkeletonMeshContent::ToContentBuffer()
{
    // m_pStaticMemory has created with import function
    return true;
}

bool WESkeletonMeshContent::FromContentBuffer()
{    
    if (!m_pStaticMemory)
        return false;

    // check with head
    CopyMemory(&m_Head, m_pStaticMemory, sizeof(HEAD_SKELETON));

    // FORMAT OF THE STATIC MEMORY
    // |--------------------------
    // |   Head         
    // |--------------------------    
    // |   VB Data            
    // |--------------------------
    // |   IB Data             
    // |--------------------------
    // |   Animation Info
    // |--------------------------
    // |   Material Content
    // |--------------------------


    return false;
}

bool WESkeletonMeshContent::Import(WEFbxImporter* pImporter)
{
    if (m_pStaticMemory)
    {
        SAFE_DELETE(m_pStaticMemory);
    }

    const vector <MESH_SUBSET>* pSubset = &pImporter->GetMeshSubsets();
    const vector <MATERIAL>* pMaterials = &pImporter->GetMaterials();
    const vector <VERTEX_SKELETON>* pVertices = &pImporter->GetSkeletonVertices();
    const vector <DWORD>* pIndices = &pImporter->GetIndices();
    m_AnimationInfo.Init(pImporter->GetAnimationInfo());
    
    // clear material content vector
    ReleaseMaterialContents();

    // compute material
    for (int i = 0; i < pMaterials->size(); i++)
    {
        WEMaterialContent* pNewMaterial = new WEMaterialContent();
        const MATERIAL* pMtl = &pImporter->GetMaterials()[i];
        pNewMaterial->Create(pMtl);
        m_MaterialContents.push_back(pNewMaterial);
    }


    UINT sizeofVB = sizeof(VERTEX_SKELETON) * pVertices->size();
    UINT sizeofIB = sizeof(DWORD) * pIndices->size();;
    UINT sizeofAnim = m_AnimationInfo.Size();

    // Fill head structure.
    ZeroMemory(&m_Head, sizeof(HEAD_SKELETON));

    m_Head.uVertexOffset = sizeof(HEAD_SKELETON);    
    m_Head.uIndexOffset = sizeof(HEAD_SKELETON) + sizeofVB;
    m_Head.uAnimOffset = sizeof(HEAD_SKELETON) + sizeofVB + sizeofIB;
    m_Head.uMaterialOffset = m_Head.uAnimOffset + sizeofAnim;

    m_Head.mMeshInfo.uSubsetCount = pSubset->size();
    m_Head.mMeshInfo.uMaterialCount = pMaterials->size();
    m_Head.mMeshInfo.uIndexCount = pIndices->size();
    m_Head.mMeshInfo.uVertexCount = pVertices->size();

    for (int i = 0; i < pSubset->size() ; i++)
    {        
        if (i >= 20)break;
        m_Head.mSubsets[i] = (*pSubset)[i];
    }

    m_Head.uMaterialDataOffsets[0] = 0;    
    for (int i = 1; i < m_Head.mMeshInfo.uMaterialCount; i++)
    {        
        if (i >= 20)break;
        m_Head.uMaterialDataOffsets[i] = m_Head.uMaterialDataOffsets[i-1] + m_MaterialContents[i]->GetSize();
    } 


    // calculate the size of buffer
    m_uSize = sizeof(HEAD_SKELETON);
    m_uSize += sizeofVB;
    m_uSize += sizeofIB;
    m_uSize += sizeofAnim;
    for (int i = 0; i < m_Head.mMeshInfo.uMaterialCount; i++)
        m_uSize += m_MaterialContents[i]->GetSize();

    // allocate memory
    m_pStaticMemory = new BYTE[m_uSize];
    if (!m_pStaticMemory)
        return false;

    //-------------------------------
    // copy memory
    // Head
    BYTE* pBuf = m_pStaticMemory;
    CopyMemory(pBuf, &m_Head, sizeof(HEAD_SKELETON));
    pBuf += sizeof(HEAD_SKELETON);

    // VB Data
    VERTEX_SKELETON* pVertex = (VERTEX_SKELETON*)pBuf;
    //CopyMemory(pVertex, vertices.data(), sizeofVB);
    for (int i = 0; i < pVertices->size(); i++)
    {
        pVertex[i] = (*pVertices)[i];
    }
    pBuf += sizeofVB;

    // IB Data
    DWORD* pIndex = (DWORD*)pBuf;
    //CopyMemory(pVertex, indices.data(), sizeofIB);
    for (int i = 0; i < pIndices->size(); i++)
    {
        pIndex[i] = (*pIndices)[i];
    }
    pBuf += sizeofIB;

    // Animation info
    m_AnimationInfo.CopyTo(pBuf);
    pBuf += sizeofAnim;

    // MaterialContents
    for (int i = 0; i < m_MaterialContents.size(); i++)
    {
        UINT size = m_MaterialContents[i]->GetSize();
        CopyMemory(pBuf, m_MaterialContents[i]->GetMemoryPtr(), size);
        pBuf += size;
    }

    //--------------------------------
    // check finally
    UINT sub = pBuf - m_pStaticMemory;
    if (sub == m_uSize)
        return true;

    return false;
}

WESkeletonMesh* WESkeletonMeshContent::CreateMesh()
{
    if (m_pStaticMemory)
        return new WESkeletonMesh(this);

    return NULL;
}

void WESkeletonMeshContent::ReleaseMaterialContents()
{
    for (int i = 0; i < m_MaterialContents.size(); i++)
    {
        SAFE_DELETE(m_MaterialContents[i]);
    }
    m_MaterialContents.clear();
}

