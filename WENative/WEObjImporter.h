#pragma once

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

#include "WED3D.h"
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

    struct Material
    {
        WCHAR strName[MAX_PATH];

        XMFLOAT3 vAmbient;
        XMFLOAT3 vDiffuse;
        XMFLOAT3 vSpecular;

        int nShininess;
        float fAlpha;

        bool bSpecular;

        bool bTexture;
        WCHAR strTexture[MAX_PATH];
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
    vector <Material*>* GetMaterial()
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
    void  InitMaterial(Material* pMaterial);
    void  DeleteCache();

    vector <CacheEntry*> m_VertexCache;

    vector <VERTEX> m_Vertices;

    vector <DWORD> m_Indices;

    vector <DWORD> m_Attributes;

    vector <Material*> m_Materials;

    WCHAR m_strMediaDir[MAX_PATH];
};
