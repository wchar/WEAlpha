#include "WEMesh.h"

WEMesh::WEMesh(WEMeshContent* pContent)
{
    HRESULT hr;

    // MeshInfo
    CopyMemory(&m_MeshInfo, &pContent->m_Head.mMeshInfo, sizeof(MESH_INFO));
    
    // Create VB
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.ByteWidth = (UINT)( GetVBStride() * m_MeshInfo.uVertexCount );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = pContent->GetVerticesPtr();
    V(WE::D3DDevice()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

    // Create IB
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = (UINT)( sizeof(DWORD) * m_MeshInfo.uIndexCount );
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = pContent->GetIndicesPtr();
    (WE::D3DDevice()->CreateBuffer(&bd, &InitData, &m_pIndexBuffer));
    
    // Craete Materials
    ReleaseMaterials();
    vector<WEMaterialContent*>* contents = &pContent->m_MaterialContents;
    for (vector<WEMaterialContent*>::iterator it = contents->begin();
        it != contents->end(); it++ )
    {
        m_Materials.push_back( (*it)->CreateMaterial() );
    }

    // Create Subsets
    ReleaseSubsets();
    for (int i = 0; i < m_MeshInfo.uSubsetCount; i++)
    { 
        MESH_SUBSET* pSubset = new MESH_SUBSET();
        CopyMemory(pSubset, &pContent->m_Head.mSubsets[i], sizeof(MESH_SUBSET));
        m_Subsets.push_back(pSubset);
    }
}


WEMesh::~WEMesh()
{
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pVertexBuffer);

    ReleaseMaterials();
    ReleaseSubsets();
}

void WEMesh::ReleaseMaterials()
{
    for (vector<WEMaterial*>::iterator it = m_Materials.begin();
        it != m_Materials.end(); it++)
    {
        SAFE_DELETE(*it);
    }
    m_Materials.clear();
}

void WEMesh::ReleaseSubsets()
{
    for (vector<MESH_SUBSET*>::iterator it = m_Subsets.begin();
        it != m_Subsets.end(); it++)
    {
        SAFE_DELETE(*it);
    }
    m_Subsets.clear();
}

