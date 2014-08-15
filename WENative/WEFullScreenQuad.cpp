#include "WEFullScreenQuad.h"

namespace
{
    struct VERTEX
    {
        XMFLOAT3 position;
        XMFLOAT2 tex;
    };
}

WEFullScreenQuad::WEFullScreenQuad()
{
    m_pVertexShader = NULL;    
    m_pLayout = NULL;     
    m_pScreenSpaceVertexBuffer = NULL; 
}


WEFullScreenQuad::~WEFullScreenQuad()
{
    Release();
}

HRESULT WEFullScreenQuad::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    HRESULT hr;

    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pd3dImmediateContext;

    ID3DBlob* pVSBlob = NULL;

    V_RETURN(WE::CompileShaderFromFile(L"shaders\\FullScreenQuadVS.hlsl", NULL, "VSMain", "vs_5_0", &pVSBlob));

    V_RETURN(pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVertexShader));

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);
    // Create the input layout
    V_RETURN(WE::D3DDevice()->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &m_pLayout));

    SAFE_RELEASE(pVSBlob);
    // Create vertex buffer of the screen space
    VERTEX vertices[6] = {
        { XMFLOAT3( 1.0f,  1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // right top
        { XMFLOAT3( 1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // right bottom
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // left bottom
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }, // left bottom
        { XMFLOAT3(-1.0f,  1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // left top
        { XMFLOAT3( 1.0f,  1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }  // right top
    };
    V_RETURN(WE::CreateBuffer(sizeof(VERTEX), 6, vertices, &m_pScreenSpaceVertexBuffer));

    return S_OK;
}

void WEFullScreenQuad::Release()
{
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pLayout);
    SAFE_RELEASE(m_pScreenSpaceVertexBuffer);
}

void WEFullScreenQuad::Prepare()
{
    // Set VB.
    UINT Strides[1];
    UINT Offsets[1];
    ID3D11Buffer* pVB[1];
    pVB[0] = m_pScreenSpaceVertexBuffer;
    Strides[0] = sizeof(VERTEX); 
    Offsets[0] = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);

    // Set layout
    m_pImmediateContext->IASetInputLayout(m_pLayout);

    // Set topology.
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set vertex shader.
    m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
}

void WEFullScreenQuad::DrawQuad()
{
    m_pImmediateContext->Draw(6, 0);
}


