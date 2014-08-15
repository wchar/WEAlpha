#include "WEMeshContent.h"
#include "WEMesh.h"

WEMeshContent::WEMeshContent()
{

}

WEMeshContent::~WEMeshContent()
{
    ReleaseMaterialContents();
}

bool WEMeshContent::FromContentBuffer()
{
    if (!m_pStaticMemory)
        return false;

    // check with head
    CopyMemory(&m_Head, m_pStaticMemory, sizeof(HEAD_STATIC));

    // FORMAT OF THE STATIC MEMORY
    // |--------------------------
    // |   Head         
    // |--------------------------    
    // |   VB Data            
    // |--------------------------
    // |   IB Data             
    // |--------------------------
    // |   Material Content
    // |--------------------------


    return true;
}


bool WEMeshContent::ToContentBuffer()
{
    // m_pStaticMemory has created with import function
    return true;
}

WEMesh* WEMeshContent::CreateMesh()
{
    if (m_pStaticMemory)
        return new WEMesh(this);

    return NULL;
}

bool WEMeshContent::Import(WEObjImporter* pImporter)
{
    if (m_pStaticMemory)
    {
        SAFE_DELETE(m_pStaticMemory);
    }

    // get
    vector <WEObjImporter::VERTEX>* pVertices = pImporter->GetVertices();
    vector <DWORD>* pIndices = pImporter->GetIndices();
    vector <DWORD>* pAttributes = pImporter->GetAttributes();
    vector <WEObjImporter::Material*>* pMaterials = pImporter->GetMaterial();

    UINT sizeofVB = GetVBStride() * pVertices->size();
    UINT sizeofIB = sizeof(DWORD) * pIndices->size();
    
    // clear material content vector
    ReleaseMaterialContents();

    // compute material
    for (std::vector<WEObjImporter::Material*>::iterator it = pMaterials->begin(); it != pMaterials->end(); ++it)
    {
        WEMaterialContent* pNewMaterial = new  WEMaterialContent();
        pNewMaterial->Create(*it);
        m_MaterialContents.push_back(pNewMaterial);
    }

    // compute subsets
    vector<MESH_SUBSET*> subsets;
    MESH_SUBSET* pSubset = NULL;
    for (UINT iMaterial = 0; iMaterial < pAttributes->size(); iMaterial++)
    {
        if (subsets.size() > 0 && pAttributes->at(iMaterial) == subsets.back()->MaterialID)
        {
            subsets.back()->IndexCount += 3;
        }
        else
        {
            pSubset = new MESH_SUBSET();
            pSubset->IndexCount = 3;
            pSubset->IndexStart = iMaterial * 3;
            pSubset->VertexStart = 0;
            pSubset->MaterialID = pAttributes->at(iMaterial);
            subsets.push_back(pSubset);
        }
    }

    // fill head
    m_Head.uVertexOffset = sizeof(HEAD_STATIC);
    m_Head.uIndexOffset = sizeof(HEAD_STATIC) + sizeofVB;
    m_Head.uMaterialOffset = sizeof(HEAD_STATIC) + sizeofVB + sizeofIB;
    
    m_Head.mMeshInfo.uVertexCount = pVertices->size();
    m_Head.mMeshInfo.uIndexCount = pIndices->size();
    m_Head.mMeshInfo.uSubsetCount = subsets.size();    
    m_Head.mMeshInfo.uMaterialCount = m_MaterialContents.size();

    for (int i = 0; i < subsets.size() && i < 20; i++)
    {
        if (i >= 20) break;
        CopyMemory(&(m_Head.mSubsets[i]), subsets[i], sizeof(MESH_SUBSET));
    }

    m_Head.uMaterialDataOffsets[0] = 0;    
    for (int i = 1; i < m_Head.mMeshInfo.uMaterialCount; i++)
    {
        if (i >= 20) break;
        m_Head.uMaterialDataOffsets[i] = m_Head.uMaterialDataOffsets[i-1] + m_MaterialContents[i]->GetSize();
    }


    // calculate the size of buffer
    m_uSize = sizeof(HEAD_STATIC);
    m_uSize += sizeofVB;
    m_uSize += sizeofIB;
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
    CopyMemory(pBuf,&m_Head, sizeof(HEAD_STATIC));
    pBuf += sizeof(HEAD_STATIC);

    // VB Data
    VERTEX_STATIC* pVertex = (VERTEX_STATIC*)pBuf;
    for (int i = 0; i < pVertices->size(); i++)
    {
        pVertex[i].position = (*pVertices)[i].position;
        pVertex[i].normal = (*pVertices)[i].normal;
        pVertex[i].texcoord = (*pVertices)[i].texcoord;
    }
    pBuf += sizeofVB;

    // IB Data
    DWORD* pIndex = (DWORD*)pBuf;

    for (int i = 0; i < pIndices->size(); i++)
    {
        pIndex[i] = (*pIndices)[i];
    }
    pBuf += sizeofIB;

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

    SAFE_DELETE(m_pStaticMemory);
    return false;
}

bool WEMeshContent::Import(WEFbxImporter* pImporter)
{
    if (m_pStaticMemory)
    {
        SAFE_DELETE(m_pStaticMemory);
    }

    const vector <MESH_SUBSET>* pSubset = &pImporter->GetMeshSubsets();
    const vector <MATERIAL>* pMaterials = &pImporter->GetMaterials();
    const vector <VERTEX_STATIC>* pVertices = &pImporter->GetStaticVertices();
    const vector <DWORD>* pIndices = &pImporter->GetIndices();

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
    

    UINT sizeofVB = sizeof(VERTEX_STATIC) * pVertices->size();
    UINT sizeofIB = sizeof(DWORD) * pIndices->size();;
    

    // Fill head structure.
    ZeroMemory(&m_Head, sizeof(HEAD_STATIC));

    m_Head.uVertexOffset = sizeof(HEAD_STATIC);    
    m_Head.uIndexOffset = sizeof(HEAD_STATIC) + sizeofVB;
    m_Head.uMaterialOffset = sizeof(HEAD_STATIC) + sizeofVB + sizeofIB;

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
    m_uSize = sizeof(HEAD_STATIC);
    m_uSize += sizeofVB;
    m_uSize += sizeofIB;
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
    CopyMemory(pBuf,&m_Head, sizeof(HEAD_STATIC));
    pBuf += sizeof(HEAD_STATIC);

    // VB Data
    VERTEX_STATIC* pVertex = (VERTEX_STATIC*)pBuf;
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

void WEMeshContent::ReleaseMaterialContents()
{
    for (int i = 0; i < m_MaterialContents.size(); i++)
    {
        SAFE_DELETE(m_MaterialContents[i]);
    }
    m_MaterialContents.clear();
}

