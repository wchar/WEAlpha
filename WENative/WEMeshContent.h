#pragma once

#include "WED3D.h"
#include "WEMaterial.h"
#include "WEObjImporter.h"
#include "WEFbxImporter.h"
#include "WEMeshMisc.h"

class DLL_EXPORT WEMeshContent : public WEContent
{
public:
    friend class WEMesh;

    WEMeshContent();
    ~WEMeshContent();

    // override
    virtual UINT GetContentType()
    { 
        return eMesh;
    }

    // Read data form m_pStaticMemory.
    virtual bool ToContentBuffer();

    // Writer data on m_pStaticMemory.
    virtual bool FromContentBuffer();

    // create by obj importer
    bool Import(WEObjImporter* pImporter);

    // Create by FBX importer.
    bool Import(WEFbxImporter* pImporter);

    // Create new mesh instance.
    WEMesh* CreateMesh();


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
    BYTE* GetMaterialsPtr()
    {
        return m_pStaticMemory + m_Head.uMaterialOffset;
    }
    UINT GetVBStride()
    {
        return sizeof(VERTEX_STATIC);
    }
    HEAD_STATIC m_Head;
    vector<WEMaterialContent*> m_MaterialContents;
};
