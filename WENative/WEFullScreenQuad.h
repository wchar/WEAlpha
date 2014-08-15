#pragma once

#include "WED3D.h"

class DLL_EXPORT WEFullScreenQuad
{
public:
    WEFullScreenQuad();
    ~WEFullScreenQuad();

    HRESULT     Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
    void        Release();
    void        Prepare();
    void        DrawQuad();
protected:
    ID3D11Device*           m_pd3dDevice;
    ID3D11DeviceContext*    m_pImmediateContext;

    ID3D11VertexShader*     m_pVertexShader;    
    ID3D11InputLayout*      m_pLayout;    
    ID3D11Buffer*           m_pScreenSpaceVertexBuffer;
};

