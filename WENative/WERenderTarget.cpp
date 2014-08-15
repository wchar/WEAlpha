#include "WERenderTarget.h"


WERenderTarget::WERenderTarget()
{
    for (int i=0; i<TARGET_COUNT; i++)
    {
        m_pRenderTargetTextureArray[i] = 0;
        m_pRenderTargetViewArray[i] = 0;
        m_pShaderResourceViewArray[i] = 0;
    }

    m_pDepthStencilBuffer = 0;
    m_pDepthStencilView = 0;
}


WERenderTarget::~WERenderTarget()
{
    Release();
}

HRESULT WERenderTarget::Create()
{
    HRESULT hr;
    D3D11_TEXTURE2D_DESC textureDesc;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ID3D11Device* device = WE::D3DDevice();

    // Get the width and height.
    UINT  textureWidth, textureHeight;
    WE::D3D().GetScreen(&textureWidth, &textureHeight);

    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target textures.
    for (int i=0; i<TARGET_COUNT; i++)
    {
        V_RETURN(device->CreateTexture2D(&textureDesc, NULL, &m_pRenderTargetTextureArray[i]));
    }

    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target views.
    for (int i=0; i<TARGET_COUNT; i++)
    {
        V_RETURN(device->CreateRenderTargetView(m_pRenderTargetTextureArray[i], &renderTargetViewDesc, &m_pRenderTargetViewArray[i]));
    }

    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource views.
    for (int i=0; i<TARGET_COUNT; i++)
    {
        V_RETURN(device->CreateShaderResourceView(m_pRenderTargetTextureArray[i], &shaderResourceViewDesc, &m_pShaderResourceViewArray[i]));
    }

    // Initialize the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = textureWidth;
    depthBufferDesc.Height = textureHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    V_RETURN(device->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer));

    // Initailze the depth stencil view description.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    V_RETURN(device->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView));

    // Setup the viewport for rendering.
    m_pViewport.Width = (float)textureWidth;
    m_pViewport.Height = (float)textureHeight;
    m_pViewport.MinDepth = 0.0f;
    m_pViewport.MaxDepth = 1.0f;
    m_pViewport.TopLeftX = 0.0f;
    m_pViewport.TopLeftY = 0.0f;

    return S_OK;
}

void WERenderTarget::Release()
{
    for (int i=0; i<TARGET_COUNT; i++)
    {
        SAFE_RELEASE(m_pRenderTargetTextureArray[i]);
        SAFE_RELEASE(m_pRenderTargetViewArray[i]);
        SAFE_RELEASE(m_pShaderResourceViewArray[i]);
    }
    SAFE_RELEASE(m_pDepthStencilBuffer);
    SAFE_RELEASE(m_pDepthStencilView);
}

void WERenderTarget::Clear()
{
    float color[4] = {0,0,0,1};
    // Clear the render target buffers.
    for (int i=0; i<TARGET_COUNT; i++)
    {
        WE::ImmediateContext()->ClearRenderTargetView(m_pRenderTargetViewArray[i], color);
    }

    // Clear the depth buffer.
    WE::ImmediateContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void WERenderTarget::SetRenderTarget()
{
    // Bind the render target view array and depth stencil buffer to the output render pipeline.
    WE::ImmediateContext()->OMSetRenderTargets(TARGET_COUNT, m_pRenderTargetViewArray, m_pDepthStencilView);

    // Set the viewport.
    WE::ImmediateContext()->RSSetViewports(1, &m_pViewport);
}