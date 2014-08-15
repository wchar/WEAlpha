#include "WEHDAO.h"

namespace
{
    struct HDAOBufferType
    {
        float fRTWidth;                  // Used by HDAO shaders for scaling texture coords
        float fRTHeight;

        float fHDAORejectRadius;          // HDAO param
        float fHDAOIntensity;             // HDAO param
        float fHDAOAcceptRadius;          // HDAO param

        float fQ;                         // far / (far - near)
        float fQTimesZNear;               // Q * near
        float fNormalScale;               // Normal scale
        float fAcceptAngle;               // Accept angle

        float fPadding[3];
    };

    struct VERTEX
    {
        XMFLOAT3 position;
        XMFLOAT2 tex;
    };
}

WEHDAO::WEHDAO()
{
    m_pPixelShader = NULL;
    m_pSamplePoint = NULL;
    m_pShaderBuffer = NULL;

    m_pTexOcclusionMap = NULL;
    m_pTexOcclusionMapRV = NULL;
    m_pTexOcclusionMapRTV = NULL;

    m_HDAOParams.fRejectRadius = 0.43f;
    m_HDAOParams.fAcceptRadius = 0.00312f;
    m_HDAOParams.fIntensity = 2.14f;
    m_HDAOParams.fNormalScale = 0.3f;
    m_HDAOParams.fAcceptAngle = 0.98f;
}


WEHDAO::~WEHDAO()
{
    Release();
}

HRESULT WEHDAO::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    HRESULT hr;

    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pd3dImmediateContext;

    // Create pixel shader.
    ID3DBlob* pPSBlob = NULL;
    V_RETURN(WE::CompileShaderFromFile(L"shaders\\HDAO.hlsl", NULL, "PSMain", "ps_5_0", &pPSBlob));
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

    D3D11_BUFFER_DESC bufferDesc;
    // Setup the description of the light dynamic constant buffer that is in the pixel shader.
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(HDAOBufferType);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
    V_RETURN(pd3dDevice->CreateBuffer(&bufferDesc, NULL, &m_pShaderBuffer));

    // Create views.
    V_RETURN(CreateViews());

    return S_OK;
}

HRESULT WEHDAO::CreateViews()
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
    V_RETURN(m_pd3dDevice->CreateTexture2D(&tmdesc, NULL, &m_pTexOcclusionMap));

    // Create the render target view
    D3D11_RENDER_TARGET_VIEW_DESC DescRT;
    DescRT.Format = tmdesc.Format;
    DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    DescRT.Texture2D.MipSlice = 0;
    V_RETURN(m_pd3dDevice->CreateRenderTargetView(m_pTexOcclusionMap, &DescRT, &m_pTexOcclusionMapRTV));

    // Create the shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
    DescRV.Format = tmdesc.Format;
    DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    DescRV.Texture2D.MipLevels = 1;
    DescRV.Texture2D.MostDetailedMip = 0;
    V_RETURN(m_pd3dDevice->CreateShaderResourceView(m_pTexOcclusionMap, &DescRV, &m_pTexOcclusionMapRV));

    return S_OK;
}

void WEHDAO::Release()
{
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pSamplePoint);
    SAFE_RELEASE(m_pShaderBuffer);

    ReleaseViews();
}

void WEHDAO::ReleaseViews()
{
    SAFE_RELEASE(m_pTexOcclusionMap);
    SAFE_RELEASE(m_pTexOcclusionMapRV);
    SAFE_RELEASE(m_pTexOcclusionMapRTV);
}

void WEHDAO::Process(WEFullScreenQuad* pFullScreenQuad, 
    ID3D11ShaderResourceView* pNormalSRV, ID3D11ShaderResourceView* pDepthSRV)
{
    HRESULT hr;

    // Prepare full screen quad.
    pFullScreenQuad->Prepare();

    // Set render target.
    m_pImmediateContext->OMSetRenderTargets(1, &m_pTexOcclusionMapRTV, NULL);

    // Set pixel shader.
    m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);

    // Set PS buffer.
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V(m_pImmediateContext->Map(m_pShaderBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
    HDAOBufferType* pBufferType = (HDAOBufferType*)MappedResource.pData;
    UINT fRTWidth,fRTHeight;
    WE::D3D().GetScreen(&fRTWidth, &fRTHeight);
    pBufferType->fRTWidth = fRTWidth;
    pBufferType->fRTHeight = fRTHeight;
    float fNear = 0.1f;
    float fFar = 20.0f;
    float fQ = fFar / (fFar - fNear);
    pBufferType->fQ = fQ;
    pBufferType->fQTimesZNear = fQ / fNear;

    pBufferType->fAcceptAngle = m_HDAOParams.fAcceptAngle;
    pBufferType->fHDAOAcceptRadius = m_HDAOParams.fAcceptRadius;
    pBufferType->fHDAOIntensity = m_HDAOParams.fIntensity;
    pBufferType->fHDAORejectRadius = m_HDAOParams.fRejectRadius;
    pBufferType->fNormalScale = m_HDAOParams.fNormalScale;

    m_pImmediateContext->Unmap(m_pShaderBuffer, 0);
    m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pShaderBuffer);

    // Set sampler.
    m_pImmediateContext->PSSetSamplers( 0, 1, &m_pSamplePoint);

    // Set PS resource views.
    m_pImmediateContext->PSSetShaderResources(0, 1, &pDepthSRV);
    m_pImmediateContext->PSSetShaderResources(1, 1, &pNormalSRV);

    // Draw.
    pFullScreenQuad->DrawQuad();
}