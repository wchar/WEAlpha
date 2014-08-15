#include "WEDeferredLighting.h"

namespace
{
    struct LightBufferType
    {
        XMFLOAT3 lightDirection;
        float padding;
    };

    struct VERTEX
    {
        XMFLOAT3 position;
        XMFLOAT2 tex;
    };
}

WEDeferredLighting::WEDeferredLighting()
{
    m_pPixelShader = NULL;
    m_pSamplePoint= NULL;
    m_pShaderBuffer = NULL;

    m_pTexLightMap = NULL;
    m_pTexLightMapRV = NULL;
    m_pTexLightMapRTV = NULL;
}


WEDeferredLighting::~WEDeferredLighting()
{
    Release();
}

HRESULT WEDeferredLighting::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    HRESULT hr;

    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pd3dImmediateContext;

    // Create pixel shader.
    ID3DBlob* pPSBlob = NULL;
    V_RETURN(WE::CompileShaderFromFile(L"shaders\\DeferredLighting.hlsl", NULL, "PSMain", "ps_5_0", &pPSBlob));
    V_RETURN(pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPixelShader));
    SAFE_RELEASE(pPSBlob);


    // Create a wrap texture sampler state description.
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    V_RETURN(pd3dDevice->CreateSamplerState(&samplerDesc, &m_pSamplePoint));

    D3D11_BUFFER_DESC lightBufferDesc;
    // Setup the description of the light dynamic constant buffer that is in the pixel shader.
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
    V_RETURN(pd3dDevice->CreateBuffer(&lightBufferDesc, NULL, &m_pShaderBuffer));

    // Create views.
    V_RETURN(CreateViews());

    return S_OK;
}

HRESULT WEDeferredLighting::CreateViews()
{
    HRESULT hr;

    ReleaseViews();

    UINT width, height;
    WE::D3D().GetScreen(&width, &height);

    D3D11_TEXTURE2D_DESC tmdesc;
    ZeroMemory(&tmdesc, sizeof(D3D11_TEXTURE2D_DESC));
    tmdesc.ArraySize = 1;
    tmdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    tmdesc.Usage = D3D11_USAGE_DEFAULT;
    tmdesc.Format = DXGI_FORMAT_R32_FLOAT;
    tmdesc.Width = width;
    tmdesc.Height = height;
    tmdesc.MipLevels = 1;
    tmdesc.SampleDesc.Count = 1;
    V_RETURN(m_pd3dDevice->CreateTexture2D(&tmdesc, NULL, &m_pTexLightMap));

    // Create the render target view
    D3D11_RENDER_TARGET_VIEW_DESC DescRT;
    DescRT.Format = tmdesc.Format;
    DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    DescRT.Texture2D.MipSlice = 0;
    V_RETURN(m_pd3dDevice->CreateRenderTargetView(m_pTexLightMap, &DescRT, &m_pTexLightMapRTV));

    // Create the shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
    DescRV.Format = tmdesc.Format;
    DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    DescRV.Texture2D.MipLevels = 1;
    DescRV.Texture2D.MostDetailedMip = 0;
    V_RETURN(m_pd3dDevice->CreateShaderResourceView(m_pTexLightMap, &DescRV, &m_pTexLightMapRV));

    return S_OK;
}

void WEDeferredLighting::Release()
{
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pSamplePoint);
    SAFE_RELEASE(m_pShaderBuffer);

    ReleaseViews();
}

void WEDeferredLighting::ReleaseViews()
{
    SAFE_RELEASE(m_pTexLightMap);
    SAFE_RELEASE(m_pTexLightMapRV);
    SAFE_RELEASE(m_pTexLightMapRTV);
}

void WEDeferredLighting::Process(WEFullScreenQuad* pFullScreenQuad, ID3D11ShaderResourceView* 
    pColorSRV, ID3D11ShaderResourceView* pNormalSRV, XMFLOAT3 vLightDir)
{
    HRESULT hr;

    // Set render target.
    m_pImmediateContext->OMSetRenderTargets(1, &m_pTexLightMapRTV, NULL);

    // Prepare full screen quad.
    pFullScreenQuad->Prepare();

    // Set pixel shader.
    m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);

    // Set PS buffer.
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V(m_pImmediateContext->Map(m_pShaderBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
    LightBufferType* pLightBufferType = (LightBufferType*)MappedResource.pData;
    pLightBufferType->lightDirection = XMFLOAT3(1,1,1);
    m_pImmediateContext->Unmap(m_pShaderBuffer, 0);
    m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pShaderBuffer);

    // Set sampler.
    m_pImmediateContext->PSSetSamplers( 0, 1, &m_pSamplePoint);

    // Set PS resource views.
    m_pImmediateContext->PSSetShaderResources(0, 1, &pColorSRV);
    m_pImmediateContext->PSSetShaderResources(1, 1, &pNormalSRV);

    // Draw quad.
    pFullScreenQuad->DrawQuad();
}