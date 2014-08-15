#pragma once

#include "WED3D.h"

// WEContent is the load or save resource for render engine 
class DLL_EXPORT WEContent
{
public:       
    WEContent();
    virtual ~WEContent();

    // content type
    enum 
    {
        eUnknow = 0,
        eMesh,
        eSkeletonMesh,
        eMaterial,
        eParticle,
        eTexture,
        eBeam,
        eAnimTrail,
    };

    // override
    virtual UINT GetContentType()
    { 
        return eUnknow;
    }
    // read data from m_pStaticMemory
    virtual bool ToContentBuffer() = 0;
    // writer data on m_pStaticMemory
    virtual bool FromContentBuffer() = 0;

    // load 
    bool Load(WCHAR* wstrFile);
    // save
    bool Save(WCHAR* wstrFile = NULL);
    // create from memory
    bool Create(BYTE* pBuf, UINT uSize);

    BYTE* GetMemoryPtr()
    {
        return m_pStaticMemory;
    }

    UINT GetSize()
    {
        return m_uSize;
    }
protected:
    BYTE*   m_pStaticMemory;
    UINT    m_uSize;

    WCHAR   m_wstrFileLocation[MAX_PATH];
};

