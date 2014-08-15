#include "WEHDR.h"

WEHDR::WEHDR()
{    
    m_pPSDownScale2x2Lum = NULL;
    m_pPSDownScale3x3 = NULL;
    m_pSamplePoint= NULL;

    for (int i = 0; i < NUM_TONEMAP_TEXTURES; i++)
    {
        m_pTexToneMapRTV[i] = NULL;
        m_pTexToneMap[i] = NULL;
        m_pTexToneMapRV[i] = NULL;
    }
}


WEHDR::~WEHDR()
{
    Release();
}


HRESULT WEHDR::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    HRESULT hr;

    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pd3dImmediateContext;

    // Create views.
    V_RETURN(CreateViews());

    // Create shaders.
    ID3DBlob* pPSBlob2x2 = NULL;
    ID3DBlob* pPSBlob3x3 = NULL;

    V_RETURN(WE::CompileShaderFromFile(L"shaders\\HDR.hlsl", NULL, "DownScale2x2_Lum", "ps_5_0", &pPSBlob2x2));
    V_RETURN(WE::CompileShaderFromFile(L"shaders\\HDR.hlsl", NULL, "DownScale3x3", "ps_5_0", &pPSBlob3x3));

    V_RETURN(pd3dDevice->CreatePixelShader(pPSBlob2x2->GetBufferPointer(), pPSBlob2x2->GetBufferSize(), NULL, &m_pPSDownScale2x2Lum));
    V_RETURN(pd3dDevice->CreatePixelShader(pPSBlob3x3->GetBufferPointer(), pPSBlob3x3->GetBufferSize(), NULL, &m_pPSDownScale3x3));

    SAFE_RELEASE(pPSBlob2x2);
    SAFE_RELEASE(pPSBlob3x3);

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
    V_RETURN(WE::D3DDevice()->CreateSamplerState(&samplerDesc, &m_pSamplePoint));

    return S_OK;
}

void WEHDR::Release()
{
    SAFE_RELEASE(m_pPSDownScale2x2Lum);
    SAFE_RELEASE(m_pPSDownScale3x3);
    SAFE_RELEASE(m_pSamplePoint);

    ReleaseViews();
}

HRESULT WEHDR::CreateViews()
{
    HRESULT hr;

    if (!m_pd3dDevice)
        return S_FALSE;

    ReleaseViews();

    int nSampleLen = 1;
    for(int i = 0; i < NUM_TONEMAP_TEXTURES; i++)
    {
        D3D11_TEXTURE2D_DESC tmdesc;
        ZeroMemory(&tmdesc, sizeof(D3D11_TEXTURE2D_DESC));
        tmdesc.ArraySize = 1;
        tmdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        tmdesc.Usage = D3D11_USAGE_DEFAULT;
        tmdesc.Format = DXGI_FORMAT_R32_FLOAT;
        tmdesc.Width = nSampleLen;
        tmdesc.Height = nSampleLen;
        tmdesc.MipLevels = 1;
        tmdesc.SampleDesc.Count = 1;
        V_RETURN(m_pd3dDevice->CreateTexture2D(&tmdesc, NULL, &m_pTexToneMap[i]));

        // Create the render target view
        D3D11_RENDER_TARGET_VIEW_DESC DescRT;
        DescRT.Format = tmdesc.Format;
        DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        DescRT.Texture2D.MipSlice = 0;
        V_RETURN(m_pd3dDevice->CreateRenderTargetView(m_pTexToneMap[i], &DescRT, &m_pTexToneMapRTV[i]));

        // Create the shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
        DescRV.Format = tmdesc.Format;
        DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        DescRV.Texture2D.MipLevels = 1;
        DescRV.Texture2D.MostDetailedMip = 0;
        V_RETURN(m_pd3dDevice->CreateShaderResourceView(m_pTexToneMap[i], &DescRV, &m_pTexToneMapRV[i]));

        nSampleLen *= 3;
    }

    return S_OK;
}

void WEHDR::ReleaseViews()
{
    for (int i = 0; i < NUM_TONEMAP_TEXTURES; i++)
    {
        SAFE_RELEASE(m_pTexToneMapRTV[i]);
        SAFE_RELEASE(m_pTexToneMap[i]);
        SAFE_RELEASE(m_pTexToneMapRV[i]);
    }
}

void WEHDR::Process(WEFullScreenQuad* pFuffScreenQuad, ID3D11ShaderResourceView* pColorRV)
{
    // Save the old viewport
    D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];    
    UINT nViewPorts = 1;
    m_pImmediateContext->RSGetViewports(&nViewPorts, vpOld); 


    // Set RTV and PS resource.  
    ID3D11ShaderResourceView* pTexSrc = pColorRV;
    ID3D11RenderTargetView* pSurfDest = m_pTexToneMapRTV[NUM_TONEMAP_TEXTURES - 1];


    m_pImmediateContext->OMSetRenderTargets(1, &pSurfDest, NULL); 

    m_pImmediateContext->PSSetShaderResources(0, 1, &pTexSrc);

    // Setup the viewport to match the backbuffer.    
    D3D11_TEXTURE2D_DESC descDest;
    m_pTexToneMap[NUM_TONEMAP_TEXTURES - 1]->GetDesc(&descDest);

    D3D11_VIEWPORT vp;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0; 
    vp.Width = (float)descDest.Width;
    vp.Height = (float)descDest.Height;
    m_pImmediateContext->RSSetViewports(1, &vp);


    // Set pixel shader.
    m_pImmediateContext->PSSetShader(m_pPSDownScale2x2Lum, 0, 0);

    // Set sampler.
    m_pImmediateContext->PSSetSamplers( 0, 1, &m_pSamplePoint);

    // Set vertex shader, quad buffer, full screen layout.
    pFuffScreenQuad->Prepare();

    // Draw quad.   
    m_pImmediateContext->Draw(6, 0);


    // Iterate through the remaining tone map textures
    for(int i = NUM_TONEMAP_TEXTURES - 1; i > 0; i--)
    {
        // Cycle the textures.Set RTV and PS resource. 
        pTexSrc = m_pTexToneMapRV[i];
        pSurfDest = m_pTexToneMapRTV[i - 1];

        m_pImmediateContext->OMSetRenderTargets(1, &pSurfDest, NULL);

        m_pImmediateContext->PSSetShaderResources(0, 1, &pTexSrc);

        // Setup the viewport to match the backbuffer.
        m_pTexToneMap[i-1]->GetDesc(&descDest);
        vp.Width = (float)descDest.Width;
        vp.Height = (float)descDest.Height;
        m_pImmediateContext->RSSetViewports(1, &vp);

        // Set pixel shader.
        m_pImmediateContext->PSSetShader(m_pPSDownScale3x3, 0, 0);

        // Draw quad again.
        m_pImmediateContext->Draw(6, 0);
    }

    ID3D11ShaderResourceView* ppSRVNULL[1] = { NULL };
    m_pImmediateContext->PSSetShaderResources(0, 1, ppSRVNULL);

    // Restore the Old viewport
    m_pImmediateContext->RSSetViewports(nViewPorts, vpOld);
}
