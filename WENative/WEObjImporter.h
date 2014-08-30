#pragma once

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

#include "WED3D.h"
#include "WEMeshMisc.h"
#include <vector>

using namespace std;

class DLL_EXPORT WEObjImporter
{
public:
    WEObjImporter(const char* cstrFileName);
    WEObjImporter(const WCHAR* wstrFileName);
    ~WEObjImporter();

public:
    struct VERTEX
    {
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT2 texcoord;
    };

    struct CacheEntry
    {
        UINT index;
        CacheEntry* pNext;
    };

public:
    vector <VERTEX>* GetVertices()
    { 
        return &m_Vertices; 
    }
    vector <DWORD>* GetIndices()
    { 
        return &m_Indices; 
    }
    vector <MATERIAL*>* GetMaterial()
    { 
        return &m_Materials; 
    }
    vector <DWORD>* GetAttributes()
    {
        return &m_Attributes; 
    }

private:
    HRESULT  LoadGeometryFromOBJ(const WCHAR* strFileName);
    HRESULT  LoadMaterialsFromMTL(const WCHAR* strFileName);
    DWORD    AddVertex(UINT hash, VERTEX* pVertex);
    void  InitMaterial(MATERIAL* pMaterial);
    void  DeleteCache();

    vector <CacheEntry*> m_VertexCache;

    vector <VERTEX> m_Vertices;

    vector <DWORD> m_Indices;

    vector <DWORD> m_Attributes;

    vector <MATERIAL*> m_Materials;

    WCHAR m_strMediaDir[MAX_PATH];
};
