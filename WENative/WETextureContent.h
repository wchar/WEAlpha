#pragma once

#include "WEContent.h"

class DLL_EXPORT WETextureContent : public WEContent
{    
public:
    friend class WETexture;
public:    
    WETextureContent();
    ~WETextureContent();
    // override
    virtual UINT GetContentType()
    { 
        return eTexture;
    }
    // read data from m_pStaticMemory
    virtual bool ToContentBuffer();
    // writer data on m_pStaticMemory
    virtual bool FromContentBuffer();

    // import from image file
    bool Import(const WCHAR* wstrFile);
    // 
    WETexture* CreateTexture();
protected:
    // texture will be stored as a ID3D11Resource and a memory buffer;
    ID3D11Resource* m_pTexture;
};

