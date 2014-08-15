#pragma once

#include "WED3D.h"
#include "WEMaterial.h"
#include "WESkeletonMeshContent.h"

class DLL_EXPORT WESkeletonMesh
{
public:
    WESkeletonMesh(WESkeletonMeshContent* pContent);
    ~WESkeletonMesh();

    // getters
    UINT GetVBStride()       
    { 
        return sizeof(VERTEX_SKELETON);
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

    XMMATRIX* GetMatrixArray()
    {
        return m_MatrixFinal;
    }

    UINT GetJointCount()
    {
        return m_AnimationInfo.uJointCount;
    }

    void TransPose(UINT iFrame);
    void TransformMesh(double fTime);
    void TransformFrame(UINT iFrame, double fTime);
private:
    void ReleaseMaterials();
    void ReleaseSubsets();

    WEMeshContent* m_pMeshContent;

    MESH_INFO m_MeshInfo;

    ID3D11Buffer* m_pVertexBuffer;
    ID3D11Buffer* m_pIndexBuffer;

    vector <MESH_SUBSET*> m_Subsets;
    vector <WEMaterial*> m_Materials;

    WEAnimation m_AnimationInfo;

    XMMATRIX m_MatrixOff[80];
    XMMATRIX m_Transform[80];
    XMMATRIX m_MatrixFinal[80];
    UINT FindAnimMatrix(double time);
};

