#pragma once

#include "WED3D.h"
#include "WEMeshMisc.h"

class DLL_EXPORT WEFbxImporter
{
public:
    WEFbxImporter(WCHAR* strFileName);
    ~WEFbxImporter();


public:
    // Getters
    const vector <VERTEX_STATIC>& GetStaticVertices() const
    {
        return m_StaticVertices;
    }

    const vector <VERTEX_SKELETON>& GetSkeletonVertices() const
    {
        return m_SkeletonVertices;
    }

    const vector <DWORD>& GetIndices() const
    {
        return m_Indices;
    }

    const vector <MESH_SUBSET>& GetMeshSubsets() const
    {
        return m_Subsets;
    }

    const vector <MATERIAL>& GetMaterials() const
    {
        return m_Materials;
    }

    const WEAnimation& GetAnimationInfo() const
    {
        return m_AnimtationInfo;
    }
protected:
    vector <VERTEX_STATIC> m_StaticVertices;
    vector <VERTEX_SKELETON> m_SkeletonVertices;
    vector <DWORD> m_Indices;
    vector <MESH_SUBSET> m_Subsets;
    vector <MATERIAL> m_Materials;
        
    WEAnimation m_AnimtationInfo;

    struct CacheEntry
    {
        UINT index;
        CacheEntry* pNext;
    };
    
    vector <CacheEntry*> m_VertexCache;
    void ReleaseCacheEntry();
    DWORD AddVertex(UINT hash, VERTEX_STATIC* pVertex, VERTEX_SKELETON* pVertex_S);

    void Create(WCHAR* strFileName);
    bool m_bResult;

    UINT m_uVertexCount;
    UINT m_uIndexCount;
};

