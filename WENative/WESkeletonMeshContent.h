#pragma once

#include "WED3D.h"
#include "WEMeshContent.h"

class DLL_EXPORT WESkeletonMeshContent : public WEContent
{
public:
    friend class WESkeletonMesh;

    WESkeletonMeshContent();
    ~WESkeletonMeshContent();

    // override
    virtual UINT GetContentType()
    { 
        return eSkeletonMesh;
    }

    // Read data form m_pStaticMemory.
    virtual bool ToContentBuffer();

    // Writer data on m_pStaticMemory.
    virtual bool FromContentBuffer();

    // Create by FBX importer.
    bool Import(WEFbxImporter* pImporter);

    // Create new mesh instance.
    WESkeletonMesh* CreateMesh();


protected:
    bool LoadMaterials();
    void ReleaseMaterialContents();

    // get ptr 
    BYTE* GetVerticesPtr()
    {
        return m_pStaticMemory + m_Head.uVertexOffset;
    }
    BYTE* GetIndicesPtr()
    {
        return m_pStaticMemory + m_Head.uIndexOffset;
    }
    BYTE* GetAnimPtr()
    {
        return m_pStaticMemory + m_Head.uAnimOffset;
    }    
    BYTE* GetMaterialsPtr()
    {
        return m_pStaticMemory + m_Head.uMaterialOffset;
    }
    UINT GetVBStride()
    {
        return sizeof(VERTEX_SKELETON);
    }
    HEAD_SKELETON m_Head;
    WEAnimation m_AnimationInfo;
    vector<WEMaterialContent*> m_MaterialContents;
};

