#pragma once

#include "WED3D.h"
#include "WEMaterial.h"
#include "WEMeshContent.h"

class DLL_EXPORT WEMesh
{
public:
    WEMesh(WEMeshContent* pContent);
    ~WEMesh();

    // getters
    UINT GetVBStride()       
    { 
        return sizeof(VERTEX_STATIC);
    }

    UINT GetIndexCount()    
    { 
        return m_MeshInfo.uIndexCount; 
    }

    ID3D11Buffer* GetVB()	  
    { 
        return m_pVertexBuffer; 
    }

    ID3D11Buffer* GetIB()    
    { 
        return m_pIndexBuffer;
    }

    vector <MESH_SUBSET*>* GetSubsets()
    { 
        return &m_Subsets; 
    }

    vector <WEMaterial*>* GetMaterials()
    {
        return &m_Materials; 
    }
private:
    void ReleaseMaterials();
    void ReleaseSubsets();

    WEMeshContent* m_pMeshContent;

    MESH_INFO m_MeshInfo;

    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pIndexBuffer;

    vector <MESH_SUBSET*> m_Subsets;
    vector <WEMaterial*> m_Materials;
};

