#include "WECascadeShadowManager.h"
#include "WEBaseCamera.h"

static const INT MAX_BUFFER_SIZE = 2048;

static const XMVECTORF32 g_vFLTMAX = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
static const XMVECTORF32 g_vFLTMIN = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
static const XMVECTORF32 g_vHalfVector = { 0.5f, 0.5f, 0.5f, 0.5f };
static const XMVECTORF32 g_vMultiplySetzwToZero = { 1.0f, 1.0f, 0.0f, 0.0f };
static const XMVECTORF32 g_vZero = { 0.0f, 0.0f, 0.0f, 0.0f };

WECascadeManager::WECascadeManager()
{
    m_pCascadeConstantBuffer;

    m_pvsQuadBlur = NULL;
    m_pvsQuadBlurBlob = NULL;
    for (int index = 0; index < MAXIMUM_BLUR_LEVELS; ++ index) 
    {    
        m_ppsQuadBlurX[index] = NULL;
        m_ppsQuadBlurY[index] = NULL;
        m_ppsQuadBlurXBlob[index] = NULL;
        m_ppsQuadBlurYBlob[index] = NULL;
    }

    m_prsScene = 0;
    m_prsShadow = 0;

    m_pSamLinear = 0;
    m_pSamShadowPoint = 0;
    m_pSamShadowLinear = 0;
    m_pSamShadowAnisotropic16 = 0;
    m_pSamShadowAnisotropic8 = 0;
    m_pSamShadowAnisotropic4 = 0;
    m_pSamShadowAnisotropic2 = 0;

    m_pCascadedShadowMapVarianceTextureArray = 0;
    for (int i = 0; i < MAX_CASCADES; i++)
    {
        m_pCascadedShadowMapVarianceRTVArrayAll[i] = 0;
        m_pCascadedShadowMapVarianceSRVArrayAll[i] = 0;
    }

    m_pCascadedShadowMapVarianceSRVArraySingle = 0;

    m_pTemporaryShadowDepthBufferTexture = 0;
    m_pTemporaryShadowDepthBufferDSV = 0;

    m_pCascadedShadowMapTempBlurTexture = 0;
    m_pCascadedShadowMapTempBlurRTV = 0;
    m_pCascadedShadowMapTempBlurSRV = 0;

    for (int i=0; i<TARGET_COUNT; i++)
    {
        m_pRenderTargetTextureArray[i] = 0;
        m_pRenderTargetViewArray[i] = 0;
        m_pShaderResourceViewArray[i] = 0;
    }

    m_pDepthBuffer = 0;
    m_pDepthStencilView = 0;

    m_pMeshRenderer = new WEMeshRenderer();
    m_pSkeletonMeshRenderer = new WESkeletonMeshRenderer();

    //
    m_CascadeConfig.m_nCascadeLevels = 4;
    m_CascadeConfig.m_iBufferSize = 512;

    m_CascadeConfig.m_iCascadePartitionsZeroToOne[0] = 13;
    m_CascadeConfig.m_iCascadePartitionsZeroToOne[1] = 30;
    m_CascadeConfig.m_iCascadePartitionsZeroToOne[2] = 60;
    m_CascadeConfig.m_iCascadePartitionsZeroToOne[3] = 100;
    m_CascadeConfig.m_iCascadePartitionsZeroToOne[4] = 30;
    m_CascadeConfig.m_iCascadePartitionsZeroToOne[5] = 100;
    m_CascadeConfig.m_iCascadePartitionsZeroToOne[6] = 100;
    m_CascadeConfig.m_iCascadePartitionsZeroToOne[7] = 100;

    m_CascadeConfig.m_iCascadePartitionsMax = 100;
    m_CascadeConfig.m_iShadowBlurSize = 13;
    m_CascadeConfig.m_iBlurBetweenCascades = 1;
    m_CascadeConfig.m_fBlurBetweenCascadesAmount = 0.002;
    m_CascadeConfig.m_bMoveLightTexelSize = false;

    for(INT index=0; index < MAX_CASCADES; ++index) 
    {
        m_RenderVP[index].Height = (FLOAT)m_CascadeConfig.m_iBufferSize;
        m_RenderVP[index].Width = (FLOAT)m_CascadeConfig.m_iBufferSize;
        m_RenderVP[index].MaxDepth = 1.0f;
        m_RenderVP[index].MinDepth = 0.0f;
        m_RenderVP[index].TopLeftX = 0;
        m_RenderVP[index].TopLeftY = 0;
    }

}

WECascadeManager::~WECascadeManager()
{
    Release();


    SAFE_DELETE(m_pMeshRenderer);
    SAFE_DELETE(m_pSkeletonMeshRenderer);
}

HRESULT WECascadeManager::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    HRESULT hr = S_OK;

    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pd3dImmediateContext;

    XMVECTOR vMeshMin;
    XMVECTOR vMeshMax;   

    // cb        
    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;

    Desc.ByteWidth = sizeof(CascadeShadowBufferType);
    V_RETURN(pd3dDevice->CreateBuffer(&Desc, NULL, &m_pCascadeConstantBuffer));


    // rs
    D3D11_RASTERIZER_DESC drd = 
    {
        D3D11_FILL_SOLID,//D3D11_FILL_MODE FillMode;
        D3D11_CULL_NONE,//D3D11_CULL_MODE CullMode;
        FALSE,//BOOL FrontCounterClockwise;
        0,//INT DepthBias;
        0.0,//FLOAT DepthBiasClamp;
        0.0,//FLOAT SlopeScaledDepthBias;
        TRUE,//BOOL DepthClipEnable;
        FALSE,//BOOL ScissorEnable;
        TRUE,//BOOL MultisampleEnable;
        FALSE//BOOL AntialiasedLineEnable;   
    };

    pd3dDevice->CreateRasterizerState(&drd, &m_prsScene);

    // Setting the slope scale depth biase greatly decreases surface acne and incorrect self shadowing.
    drd.SlopeScaledDepthBias = 1.0;
    pd3dDevice->CreateRasterizerState(&drd, &m_prsShadow);

    // samplers
    D3D11_SAMPLER_DESC SamDesc;
    SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.MipLODBias = 0.0f;
    SamDesc.MaxAnisotropy = 1;
    SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
    SamDesc.MinLOD = 0;
    SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
    V_RETURN(pd3dDevice->CreateSamplerState(&SamDesc, &m_pSamLinear));

    D3D11_SAMPLER_DESC SamDescShad = 
    {
        D3D11_FILTER_ANISOTROPIC,// D3D11_FILTER Filter;
        D3D11_TEXTURE_ADDRESS_CLAMP, //D3D11_TEXTURE_ADDRESS_MODE AddressU;
        D3D11_TEXTURE_ADDRESS_CLAMP, //D3D11_TEXTURE_ADDRESS_MODE AddressV;
        D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressW;
        0,//FLOAT MipLODBias;
        16,//UINT MaxAnisotropy;
        D3D11_COMPARISON_NEVER , //D3D11_COMPARISON_FUNC ComparisonFunc;
        0.0,0.0,0.0,0.0,//FLOAT BorderColor[ 4 ];
        0,//FLOAT MinLOD;
        0//FLOAT MaxLOD;   
    };

    V_RETURN(pd3dDevice->CreateSamplerState(&SamDescShad, &m_pSamShadowAnisotropic16));

    SamDescShad.MaxAnisotropy = 8;
    V_RETURN(pd3dDevice->CreateSamplerState(&SamDescShad, &m_pSamShadowAnisotropic8));

    SamDescShad.MaxAnisotropy = 4;
    V_RETURN(pd3dDevice->CreateSamplerState(&SamDescShad, &m_pSamShadowAnisotropic4));

    SamDescShad.MaxAnisotropy = 2;
    V_RETURN(pd3dDevice->CreateSamplerState(&SamDescShad, &m_pSamShadowAnisotropic2));

    SamDescShad.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamDescShad.MaxAnisotropy = 0;
    V_RETURN(pd3dDevice->CreateSamplerState(&SamDescShad, &m_pSamShadowLinear));

    SamDescShad.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    V_RETURN(pd3dDevice->CreateSamplerState(&SamDescShad, &m_pSamShadowPoint));


    // Create views.
    V_RETURN(ReleaseAndCreateCascadeBuffers());
    V_RETURN(CreateRenderTargetBuffers());

    // Mesh renderer.
    V_RETURN(m_pMeshRenderer->Create(pd3dDevice, pd3dImmediateContext));
    V_RETURN(m_pSkeletonMeshRenderer->Create(pd3dDevice, pd3dImmediateContext));

    // Create shaders.
    V_RETURN(CreateShaders());

    return S_OK;
}

HRESULT WECascadeManager::CreateShaders()
{
    HRESULT hr;

    if (m_pvsQuadBlurBlob == NULL) 
    {
        V_RETURN(WE::CompileShaderFromFile(L"shaders\\Variance.hlsl", NULL, "VSMain", "vs_4_0", &m_pvsQuadBlurBlob));
    }
    V_RETURN(m_pd3dDevice->CreateVertexShader(m_pvsQuadBlurBlob->GetBufferPointer(), m_pvsQuadBlurBlob->GetBufferSize(), 
        NULL, &m_pvsQuadBlur));

    INT iKernelSize = 1;
    D3D_SHADER_MACRO kernelDefines[] = 
    {
        "SEPERABLE_BLUR_KERNEL_SIZE", "1",
        NULL, NULL
    };
    char cKernelSize[10];
    cKernelSize[0] = 0;
    for (INT iBlurKernelIndex = 0; iBlurKernelIndex < MAXIMUM_BLUR_LEVELS; ++iBlurKernelIndex) {
        iKernelSize+=2;
        sprintf_s(cKernelSize, "%d", iKernelSize);
        kernelDefines[0].Definition = cKernelSize;
        if (m_ppsQuadBlurXBlob[iBlurKernelIndex] == NULL)
        {
            V_RETURN(WE::CompileShaderFromFile(
                L"shaders\\Variance.hlsl", kernelDefines, "PSBlurX", "ps_4_0", &m_ppsQuadBlurXBlob[iBlurKernelIndex]));
        }
        V_RETURN(m_pd3dDevice->CreatePixelShader(m_ppsQuadBlurXBlob[iBlurKernelIndex]->GetBufferPointer(), 
            m_ppsQuadBlurXBlob[iBlurKernelIndex]->GetBufferSize(), 
            NULL, &m_ppsQuadBlurX[iBlurKernelIndex]));

        if (m_ppsQuadBlurYBlob[iBlurKernelIndex] == NULL) 
        {
            V_RETURN(WE::CompileShaderFromFile(
                L"shaders\\Variance.hlsl", kernelDefines, "PSBlurY", "ps_4_0", &m_ppsQuadBlurYBlob[iBlurKernelIndex]));
        }
        V_RETURN(m_pd3dDevice->CreatePixelShader(
            m_ppsQuadBlurYBlob[iBlurKernelIndex]->GetBufferPointer(), 
            m_ppsQuadBlurYBlob[iBlurKernelIndex]->GetBufferSize(), 
            NULL, &m_ppsQuadBlurY[iBlurKernelIndex]));
    }


    return S_OK;
}

HRESULT WECascadeManager::CreateRenderTargetBuffers()
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
    depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    V_RETURN(device->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthBuffer));

    // Initailze the depth stencil view description.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    V_RETURN(device->CreateDepthStencilView(m_pDepthBuffer, &depthStencilViewDesc, &m_pDepthStencilView));

    // Craete the deoth shader resource view.
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    V_RETURN(device->CreateShaderResourceView(m_pDepthBuffer, &shaderResourceViewDesc, &m_pDepthShaderResourceView));

    // Setup the viewport for rendering.
    m_pViewport.Width = (float)textureWidth;
    m_pViewport.Height = (float)textureHeight;
    m_pViewport.MinDepth = 0.0f;
    m_pViewport.MaxDepth = 1.0f;
    m_pViewport.TopLeftX = 0.0f;
    m_pViewport.TopLeftY = 0.0f;

    return S_OK;
}

HRESULT WECascadeManager::ReleaseAndCreateCascadeBuffers()
{
    HRESULT hr;

    // vp
    for (INT index=0; index < m_CascadeConfig.m_nCascadeLevels; ++index) 
    { 
        m_RenderVP[index].Height = (FLOAT)m_CascadeConfig.m_iBufferSize;
        m_RenderVP[index].Width = (FLOAT)m_CascadeConfig.m_iBufferSize;
        m_RenderVP[index].MaxDepth = 1.0f;
        m_RenderVP[index].MinDepth = 0.0f;
        m_RenderVP[index].TopLeftX = (FLOAT)(m_CascadeConfig.m_iBufferSize * index);
        m_RenderVP[index].TopLeftY = 0;
    }

    m_RenderOneTileVP.Height = (FLOAT)m_CascadeConfig.m_iBufferSize;
    m_RenderOneTileVP.Width = (FLOAT)m_CascadeConfig.m_iBufferSize;
    m_RenderOneTileVP.MaxDepth = 1.0f;
    m_RenderOneTileVP.MinDepth = 0.0f;
    m_RenderOneTileVP.TopLeftX = 0.0f;
    m_RenderOneTileVP.TopLeftY = 0.0f;


    // depth
    SAFE_RELEASE(m_pCascadedShadowMapVarianceTextureArray);
    for (int index=0; index < MAX_CASCADES; ++index) {
        SAFE_RELEASE(m_pCascadedShadowMapVarianceRTVArrayAll[index]);
        SAFE_RELEASE(m_pCascadedShadowMapVarianceSRVArrayAll[index]);
    }
    SAFE_RELEASE(m_pCascadedShadowMapVarianceSRVArraySingle);

    SAFE_RELEASE(m_pCascadedShadowMapTempBlurTexture);
    SAFE_RELEASE(m_pCascadedShadowMapTempBlurRTV);
    SAFE_RELEASE(m_pCascadedShadowMapTempBlurSRV);

    SAFE_RELEASE(m_pTemporaryShadowDepthBufferTexture);
    SAFE_RELEASE(m_pTemporaryShadowDepthBufferDSV);

    D3D11_TEXTURE2D_DESC dtd = {
        m_CascadeConfig.m_iBufferSize,//UINT Width;
        m_CascadeConfig.m_iBufferSize,//UINT Height;
        1,//UINT MipLevels;
        m_CascadeConfig.m_nCascadeLevels,//UINT ArraySize;
        DXGI_FORMAT_R32G32_FLOAT,//DXGI_FORMAT Format;
        1,//DXGI_SAMPLE_DESC SampleDesc;
        0,
        D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
        0,//UINT CPUAccessFlags;
        0//UINT MiscFlags;    
    };

    V_RETURN(m_pd3dDevice->CreateTexture2D(&dtd, NULL, &m_pCascadedShadowMapVarianceTextureArray ));

    dtd.ArraySize = 1;
    V_RETURN(m_pd3dDevice->CreateTexture2D(&dtd, NULL, &m_pCascadedShadowMapTempBlurTexture ));

    dtd.Format = DXGI_FORMAT_R32_TYPELESS;
    dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    V_RETURN(m_pd3dDevice->CreateTexture2D(&dtd, NULL, &m_pTemporaryShadowDepthBufferTexture ));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Flags = 0;
    dsvd.Format = DXGI_FORMAT_D32_FLOAT;
    dsvd.Texture2D.MipSlice = 0;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    V_RETURN(m_pd3dDevice->CreateDepthStencilView(m_pTemporaryShadowDepthBufferTexture, &dsvd, &m_pTemporaryShadowDepthBufferDSV));

    D3D11_RENDER_TARGET_VIEW_DESC drtvd = {
        DXGI_FORMAT_R32G32_FLOAT,
        D3D11_RTV_DIMENSION_TEXTURE2DARRAY
    };

    drtvd.Texture2DArray.MipSlice = 0;

    for(int index = 0; index < m_CascadeConfig.m_nCascadeLevels; ++index) 
    {
        drtvd.Texture2DArray.FirstArraySlice = index;
        drtvd.Texture2DArray.ArraySize = 1;
        V_RETURN(m_pd3dDevice->CreateRenderTargetView (m_pCascadedShadowMapVarianceTextureArray, &drtvd,
            &m_pCascadedShadowMapVarianceRTVArrayAll[index]));
    }

    drtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    drtvd.Texture2D.MipSlice = 0;

    V_RETURN(m_pd3dDevice->CreateRenderTargetView (m_pCascadedShadowMapTempBlurTexture, &drtvd,
        &m_pCascadedShadowMapTempBlurRTV));

    D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd = {
        DXGI_FORMAT_R32G32_FLOAT,
        D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
    };

    dsrvd.Texture2DArray.ArraySize = m_CascadeConfig.m_nCascadeLevels;
    dsrvd.Texture2DArray.FirstArraySlice = 0;
    dsrvd.Texture2DArray.MipLevels = 1;
    dsrvd.Texture2DArray.MostDetailedMip = 0;

    V_RETURN(m_pd3dDevice->CreateShaderResourceView(m_pCascadedShadowMapVarianceTextureArray, 
        &dsrvd, &m_pCascadedShadowMapVarianceSRVArraySingle));

    for(int index = 0; index < m_CascadeConfig.m_nCascadeLevels; ++index) 
    {
        dsrvd.Texture2DArray.ArraySize = 1;
        dsrvd.Texture2DArray.FirstArraySlice = index;
        dsrvd.Texture2DArray.MipLevels = 1;
        dsrvd.Texture2DArray.MostDetailedMip = 0;
        V_RETURN(m_pd3dDevice->CreateShaderResourceView(m_pCascadedShadowMapVarianceTextureArray, 
            &dsrvd, &m_pCascadedShadowMapVarianceSRVArrayAll[index]));
    }

    //dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //dsrvd.Texture2D.MipLevels = 1;
    //dsrvd.Texture2D.MostDetailedMip = 0;
    dsrvd.Texture2DArray.ArraySize = 1;
    dsrvd.Texture2DArray.FirstArraySlice = 0;
    dsrvd.Texture2DArray.MipLevels = 1;
    dsrvd.Texture2DArray.MostDetailedMip = 0;
    V_RETURN(m_pd3dDevice->CreateShaderResourceView(m_pCascadedShadowMapTempBlurTexture, 
        &dsrvd, &m_pCascadedShadowMapTempBlurSRV));

    return S_OK;
}

void WECascadeManager::Release() 
{
    SAFE_RELEASE(m_pCascadeConstantBuffer);

    SAFE_RELEASE(m_pvsQuadBlur);
    SAFE_RELEASE(m_pvsQuadBlur);
    for (int index = 0; index < MAXIMUM_BLUR_LEVELS; ++index) {
        SAFE_RELEASE(m_ppsQuadBlurX[index]);
        SAFE_RELEASE(m_ppsQuadBlurY[index]);

        SAFE_RELEASE(m_ppsQuadBlurXBlob[index]);
        SAFE_RELEASE(m_ppsQuadBlurYBlob[index]);
    }

    SAFE_RELEASE(m_prsShadow);
    SAFE_RELEASE(m_prsScene);

    SAFE_RELEASE(m_pSamLinear);
    SAFE_RELEASE(m_pSamShadowPoint);
    SAFE_RELEASE(m_pSamShadowLinear);
    SAFE_RELEASE(m_pSamShadowAnisotropic2);
    SAFE_RELEASE(m_pSamShadowAnisotropic4);
    SAFE_RELEASE(m_pSamShadowAnisotropic8);
    SAFE_RELEASE(m_pSamShadowAnisotropic16);


    SAFE_RELEASE(m_pCascadedShadowMapVarianceTextureArray);
    for (int index=0; index < MAX_CASCADES; ++index) {
        SAFE_RELEASE(m_pCascadedShadowMapVarianceRTVArrayAll[index]);
        SAFE_RELEASE(m_pCascadedShadowMapVarianceSRVArrayAll[index]);
    }
    SAFE_RELEASE(m_pCascadedShadowMapVarianceSRVArraySingle);

    SAFE_RELEASE(m_pTemporaryShadowDepthBufferTexture);
    SAFE_RELEASE(m_pTemporaryShadowDepthBufferDSV);

    SAFE_RELEASE(m_pCascadedShadowMapTempBlurTexture);
    SAFE_RELEASE(m_pCascadedShadowMapTempBlurRTV);
    SAFE_RELEASE(m_pCascadedShadowMapTempBlurSRV);


    ReleaseRenderTargetBuffers();
}

void WECascadeManager::ReleaseRenderTargetBuffers()
{
    for (int i=0; i<TARGET_COUNT; i++)
    {
        SAFE_RELEASE(m_pRenderTargetTextureArray[i]);
        SAFE_RELEASE(m_pRenderTargetViewArray[i]);
        SAFE_RELEASE(m_pShaderResourceViewArray[i]);
    }
    SAFE_RELEASE(m_pDepthBuffer);
    SAFE_RELEASE(m_pDepthStencilView);
    SAFE_RELEASE(m_pDepthShaderResourceView);
}

namespace XNA
{

    _DECLSPEC_ALIGN_16_ struct Frustum
    {
        XMFLOAT3 Origin;        // Origin of the frustum (and projection).
        XMFLOAT4 Orientation;   // Unit quaternion representing rotation.

        FLOAT RightSlope;       // Positive X slope (X/Z).
        FLOAT LeftSlope;        // Negative X slope.
        FLOAT TopSlope;         // Positive Y slope (Y/Z).
        FLOAT BottomSlope;      // Negative Y slope.
        FLOAT Near, Far;        // Z of the near plane and far plane.
    };

    VOID ComputeFrustumFromProjection(Frustum* pOut, XMMATRIX* pProjection)
    {
        XMASSERT(pOut);
        XMASSERT(pProjection);

        // Corners of the projection frustum in homogenous space.
        static XMVECTOR HomogenousPoints[6] =
        {
            {  1.0f,  0.0f, 1.0f, 1.0f },   // right (at far plane)
            { -1.0f,  0.0f, 1.0f, 1.0f },   // left
            {  0.0f,  1.0f, 1.0f, 1.0f },   // top
            {  0.0f, -1.0f, 1.0f, 1.0f },   // bottom

            { 0.0f, 0.0f, 0.0f, 1.0f },     // near
            { 0.0f, 0.0f, 1.0f, 1.0f }      // far
        };

        XMVECTOR Determinant;
        XMMATRIX matInverse = XMMatrixInverse(&Determinant, *pProjection);

        // Compute the frustum corners in world space.
        XMVECTOR Points[6];

        for(INT i = 0; i < 6; i++)
        {
            // Transform point.
            Points[i] = XMVector4Transform(HomogenousPoints[i], matInverse);
        }

        pOut->Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
        pOut->Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

        // Compute the slopes.
        Points[0] = Points[0] * XMVectorReciprocal(XMVectorSplatZ(Points[0]));
        Points[1] = Points[1] * XMVectorReciprocal(XMVectorSplatZ(Points[1]));
        Points[2] = Points[2] * XMVectorReciprocal(XMVectorSplatZ(Points[2]));
        Points[3] = Points[3] * XMVectorReciprocal(XMVectorSplatZ(Points[3]));

        pOut->RightSlope = XMVectorGetX(Points[0]);
        pOut->LeftSlope = XMVectorGetX(Points[1]);
        pOut->TopSlope = XMVectorGetY(Points[2]);
        pOut->BottomSlope = XMVectorGetY(Points[3]);

        // Compute near and far.
        Points[4] = Points[4] * XMVectorReciprocal(XMVectorSplatW(Points[4]));
        Points[5] = Points[5] * XMVectorReciprocal(XMVectorSplatW(Points[5]));

        pOut->Near = XMVectorGetZ(Points[4]);
        pOut->Far = XMVectorGetZ(Points[5]);

        return;
    }

}


//--------------------------------------------------------------------------------------
// This function takes the camera's projection matrix and returns the 8
// points that make up a view frustum.
// The frustum is scaled to fit within the Begin and End interval paramaters.
//--------------------------------------------------------------------------------------
void CreateFrustumPointsFromCascadeInterval(float fCascadeIntervalBegin, 
    FLOAT fCascadeIntervalEnd, 
    XMMATRIX &vProjection,
    XMVECTOR* pvCornerPointsWorld) 
{

    XNA::Frustum vViewFrust;
    ComputeFrustumFromProjection(&vViewFrust, &vProjection);
    vViewFrust.Near = fCascadeIntervalBegin;
    vViewFrust.Far = fCascadeIntervalEnd;

    static const XMVECTORU32 vGrabY = {0x00000000,0xFFFFFFFF,0x00000000,0x00000000};
    static const XMVECTORU32 vGrabX = {0xFFFFFFFF,0x00000000,0x00000000,0x00000000};

    XMVECTORF32 vRightTop = {vViewFrust.RightSlope,vViewFrust.TopSlope,1.0f,1.0f};
    XMVECTORF32 vLeftBottom = {vViewFrust.LeftSlope,vViewFrust.BottomSlope,1.0f,1.0f};
    XMVECTORF32 vNear = {vViewFrust.Near,vViewFrust.Near,vViewFrust.Near,1.0f};
    XMVECTORF32 vFar = {vViewFrust.Far,vViewFrust.Far,vViewFrust.Far,1.0f};
    XMVECTOR vRightTopNear = XMVectorMultiply(vRightTop, vNear);
    XMVECTOR vRightTopFar = XMVectorMultiply(vRightTop, vFar);
    XMVECTOR vLeftBottomNear = XMVectorMultiply(vLeftBottom, vNear);
    XMVECTOR vLeftBottomFar = XMVectorMultiply(vLeftBottom, vFar);

    pvCornerPointsWorld[0] = vRightTopNear;
    pvCornerPointsWorld[1] = XMVectorSelect(vRightTopNear, vLeftBottomNear, vGrabX);
    pvCornerPointsWorld[2] = vLeftBottomNear;
    pvCornerPointsWorld[3] = XMVectorSelect(vRightTopNear, vLeftBottomNear,vGrabY);

    pvCornerPointsWorld[4] = vRightTopFar;
    pvCornerPointsWorld[5] = XMVectorSelect(vRightTopFar, vLeftBottomFar, vGrabX);
    pvCornerPointsWorld[6] = vLeftBottomFar;
    pvCornerPointsWorld[7] = XMVectorSelect(vRightTopFar ,vLeftBottomFar, vGrabY);

}

//--------------------------------------------------------------------------------------
// Used to compute an intersection of the orthographic projection and the Scene AABB
//--------------------------------------------------------------------------------------
struct Triangle 
{
    XMVECTOR pt[3];
    BOOL culled;
};

//--------------------------------------------------------------------------------------
// Computing an accurate near and flar plane will decrease surface acne and Peter-panning.
// Surface acne is the term for erroneous self shadowing.  Peter-panning is the effect where
// shadows disappear near the base of an object.
// As offsets are generally used with PCF filtering due self shadowing issues, computing the
// correct near and far planes becomes even more important.
// This concept is not complicated, but the intersection code is.
//--------------------------------------------------------------------------------------
void ComputeNearAndFar(FLOAT& fNearPlane, 
    FLOAT& fFarPlane, 
    FXMVECTOR vLightCameraOrthographicMin, 
    FXMVECTOR vLightCameraOrthographicMax, 
    XMVECTOR* pvPointsInCameraView) 
{

    // Initialize the near and far planes
    fNearPlane = FLT_MAX;
    fFarPlane = -FLT_MAX;

    Triangle triangleList[16];
    INT iTriangleCnt = 1;

    triangleList[0].pt[0] = pvPointsInCameraView[0];
    triangleList[0].pt[1] = pvPointsInCameraView[1];
    triangleList[0].pt[2] = pvPointsInCameraView[2];
    triangleList[0].culled = false;

    // These are the indices used to tesselate an AABB into a list of triangles.
    static const INT iAABBTriIndexes[] = 
    {
        0,1,2,  1,2,3,
        4,5,6,  5,6,7,
        0,2,4,  2,4,6,
        1,3,5,  3,5,7,
        0,1,4,  1,4,5,
        2,3,6,  3,6,7 
    };

    INT iPointPassesCollision[3];

    // At a high level: 
    // 1. Iterate over all 12 triangles of the AABB.  
    // 2. Clip the triangles against each plane. Create new triangles as needed.
    // 3. Find the min and max z values as the near and far plane.

    //This is easier because the triangles are in camera spacing making the collisions tests simple comparisions.

    float fLightCameraOrthographicMinX = XMVectorGetX(vLightCameraOrthographicMin);
    float fLightCameraOrthographicMaxX = XMVectorGetX(vLightCameraOrthographicMax); 
    float fLightCameraOrthographicMinY = XMVectorGetY(vLightCameraOrthographicMin);
    float fLightCameraOrthographicMaxY = XMVectorGetY(vLightCameraOrthographicMax);

    for(INT AABBTriIter = 0; AABBTriIter < 12; ++AABBTriIter) 
    {

        triangleList[0].pt[0] = pvPointsInCameraView[ iAABBTriIndexes[ AABBTriIter*3 + 0 ] ];
        triangleList[0].pt[1] = pvPointsInCameraView[ iAABBTriIndexes[ AABBTriIter*3 + 1 ] ];
        triangleList[0].pt[2] = pvPointsInCameraView[ iAABBTriIndexes[ AABBTriIter*3 + 2 ] ];
        iTriangleCnt = 1;
        triangleList[0].culled = FALSE;

        // Clip each invidual triangle against the 4 frustums.  When ever a triangle is clipped into new triangles, 
        //add them to the list.
        for(INT frustumPlaneIter = 0; frustumPlaneIter < 4; ++frustumPlaneIter) 
        {

            FLOAT fEdge;
            INT iComponent;

            if(frustumPlaneIter == 0) 
            {
                fEdge = fLightCameraOrthographicMinX; // todo make float temp
                iComponent = 0;
            } 
            else if(frustumPlaneIter == 1) 
            {
                fEdge = fLightCameraOrthographicMaxX;
                iComponent = 0;
            } 
            else if(frustumPlaneIter == 2) 
            {
                fEdge = fLightCameraOrthographicMinY;
                iComponent = 1;
            } 
            else 
            {
                fEdge = fLightCameraOrthographicMaxY;
                iComponent = 1;
            }

            for(INT triIter=0; triIter < iTriangleCnt; ++triIter) 
            {
                // We don't delete triangles, so we skip those that have been culled.
                if(!triangleList[triIter].culled) 
                {
                    INT iInsideVertCount = 0;
                    XMVECTOR tempOrder;
                    // Test against the correct frustum plane.
                    // This could be written more compactly, but it would be harder to understand.

                    if(frustumPlaneIter == 0) 
                    {
                        for(INT triPtIter=0; triPtIter < 3; ++triPtIter) 
                        {
                            if(XMVectorGetX(triangleList[triIter].pt[triPtIter]) >
                                XMVectorGetX(vLightCameraOrthographicMin)) 
                            { 
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else 
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else if(frustumPlaneIter == 1) 
                    {
                        for(INT triPtIter=0; triPtIter < 3; ++triPtIter) 
                        {
                            if(XMVectorGetX(triangleList[triIter].pt[triPtIter]) < 
                                XMVectorGetX(vLightCameraOrthographicMax))
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else
                            { 
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else if(frustumPlaneIter == 2) 
                    {
                        for(INT triPtIter=0; triPtIter < 3; ++triPtIter) 
                        {
                            if(XMVectorGetY(triangleList[triIter].pt[triPtIter]) > 
                                XMVectorGetY(vLightCameraOrthographicMin)) 
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else 
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }
                    else 
                    {
                        for(INT triPtIter=0; triPtIter < 3; ++triPtIter) 
                        {
                            if(XMVectorGetY(triangleList[triIter].pt[triPtIter]) < 
                                XMVectorGetY(vLightCameraOrthographicMax)) 
                            {
                                iPointPassesCollision[triPtIter] = 1;
                            }
                            else 
                            {
                                iPointPassesCollision[triPtIter] = 0;
                            }
                            iInsideVertCount += iPointPassesCollision[triPtIter];
                        }
                    }

                    // Move the points that pass the frustum test to the begining of the array.
                    if(iPointPassesCollision[1] && !iPointPassesCollision[0]) 
                    {
                        tempOrder =  triangleList[triIter].pt[0];   
                        triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = tempOrder;
                        iPointPassesCollision[0] = TRUE;            
                        iPointPassesCollision[1] = FALSE;            
                    }
                    if(iPointPassesCollision[2] && !iPointPassesCollision[1]) 
                    {
                        tempOrder =  triangleList[triIter].pt[1];   
                        triangleList[triIter].pt[1] = triangleList[triIter].pt[2];
                        triangleList[triIter].pt[2] = tempOrder;
                        iPointPassesCollision[1] = TRUE;            
                        iPointPassesCollision[2] = FALSE;                        
                    }
                    if(iPointPassesCollision[1] && !iPointPassesCollision[0]) 
                    {
                        tempOrder =  triangleList[triIter].pt[0];   
                        triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
                        triangleList[triIter].pt[1] = tempOrder;
                        iPointPassesCollision[0] = TRUE;            
                        iPointPassesCollision[1] = FALSE;            
                    }

                    if(iInsideVertCount == 0) 
                    { // All points failed. We're done,  
                        triangleList[triIter].culled = true;
                    }
                    else if(iInsideVertCount == 1) 
                    {// One point passed. Clip the triangle against the Frustum plane
                        triangleList[triIter].culled = false;

                        // 
                        XMVECTOR vVert0ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[0];
                        XMVECTOR vVert0ToVert2 = triangleList[triIter].pt[2] - triangleList[triIter].pt[0];

                        // Find the collision ratio.
                        FLOAT fHitPointTimeRatio = fEdge - XMVectorGetByIndex(triangleList[triIter].pt[0], iComponent) ;
                        // Calculate the distance along the vector as ratio of the hit ratio to the component.
                        FLOAT fDistanceAlongVector01 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert1, iComponent);
                        FLOAT fDistanceAlongVector02 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert2, iComponent);
                        // Add the point plus a percentage of the vector.
                        vVert0ToVert1 *= fDistanceAlongVector01;
                        vVert0ToVert1 += triangleList[triIter].pt[0];
                        vVert0ToVert2 *= fDistanceAlongVector02;
                        vVert0ToVert2 += triangleList[triIter].pt[0];

                        triangleList[triIter].pt[1] = vVert0ToVert2;
                        triangleList[triIter].pt[2] = vVert0ToVert1;

                    }
                    else if(iInsideVertCount == 2) 
                    { // 2 in  // tesselate into 2 triangles


                        // Copy the triangle\(if it exists) after the current triangle out of
                        // the way so we can override it with the new triangle we're inserting.
                        triangleList[iTriangleCnt] = triangleList[triIter+1];

                        triangleList[triIter].culled = false;
                        triangleList[triIter+1].culled = false;

                        // Get the vector from the outside point into the 2 inside points.
                        XMVECTOR vVert2ToVert0 = triangleList[triIter].pt[0] - triangleList[triIter].pt[2];
                        XMVECTOR vVert2ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[2];

                        // Get the hit point ratio.
                        FLOAT fHitPointTime_2_0 =  fEdge - XMVectorGetByIndex(triangleList[triIter].pt[2], iComponent);
                        FLOAT fDistanceAlongVector_2_0 = fHitPointTime_2_0 / XMVectorGetByIndex(vVert2ToVert0, iComponent);
                        // Calcaulte the new vert by adding the percentage of the vector plus point 2.
                        vVert2ToVert0 *= fDistanceAlongVector_2_0;
                        vVert2ToVert0 += triangleList[triIter].pt[2];

                        // Add a new triangle.
                        triangleList[triIter+1].pt[0] = triangleList[triIter].pt[0];
                        triangleList[triIter+1].pt[1] = triangleList[triIter].pt[1];
                        triangleList[triIter+1].pt[2] = vVert2ToVert0;

                        //Get the hit point ratio.
                        FLOAT fHitPointTime_2_1 =  fEdge - XMVectorGetByIndex(triangleList[triIter].pt[2], iComponent) ;
                        FLOAT fDistanceAlongVector_2_1 = fHitPointTime_2_1 / XMVectorGetByIndex(vVert2ToVert1, iComponent);
                        vVert2ToVert1 *= fDistanceAlongVector_2_1;
                        vVert2ToVert1 += triangleList[triIter].pt[2];
                        triangleList[triIter].pt[0] = triangleList[triIter+1].pt[1];
                        triangleList[triIter].pt[1] = triangleList[triIter+1].pt[2];
                        triangleList[triIter].pt[2] = vVert2ToVert1;
                        // Cncrement triangle count and skip the triangle we just inserted.
                        ++iTriangleCnt;
                        ++triIter;


                    }
                    else 
                    { // all in
                        triangleList[triIter].culled = false;

                    }
                }// end if !culled loop            
            }
        }
        for(INT index=0; index < iTriangleCnt; ++index) 
        {
            if(!triangleList[index].culled) 
            {
                // Set the near and far plan and the min and max z values respectivly.
                for(int vertind = 0; vertind < 3; ++ vertind) 
                {
                    float fTriangleCoordZ = XMVectorGetZ(triangleList[index].pt[vertind]);
                    if(fNearPlane > fTriangleCoordZ) 
                    {
                        fNearPlane = fTriangleCoordZ;
                    }
                    if(fFarPlane  <fTriangleCoordZ) 
                    {
                        fFarPlane = fTriangleCoordZ;
                    }
                }
            }
        }
    }    

}

void CreateAABBPoints(XMVECTOR* vAABBPoints, FXMVECTOR vCenter, FXMVECTOR vExtents)
{
    //This map enables us to use a for loop and do vector math.
    static const XMVECTORF32 vExtentsMap[] = 
    { 
        { 1.0f,  1.0f, -1.0f, 1.0f}, 
        {-1.0f,  1.0f, -1.0f, 1.0f}, 
        { 1.0f, -1.0f, -1.0f, 1.0f}, 
        {-1.0f, -1.0f, -1.0f, 1.0f}, 
        { 1.0f,  1.0f,  1.0f, 1.0f}, 
        {-1.0f,  1.0f,  1.0f, 1.0f}, 
        { 1.0f, -1.0f,  1.0f, 1.0f}, 
        {-1.0f, -1.0f,  1.0f, 1.0f} 
    };

    for(INT index = 0; index < 8; ++index) 
    {
        vAABBPoints[index] = XMVectorMultiplyAdd(vExtentsMap[index], vExtents, vCenter); 
    }

}


void WECascadeManager::ComputeMatrices()
{
    WEBaseCamera* pCamera = WE::Camera();
    WEBaseCamera* pLight = WE::Light();
    // Copy D3DX matricies into XNA Math matricies.

    XMMATRIX matViewCameraProjection = pCamera->GetProjMtx();
    XMMATRIX matViewCameraView = pCamera->GetViewMtx();
    XMMATRIX matLightCameraView = pLight->GetViewMtx();

    XMVECTOR det;
    XMMATRIX matInverseViewCamera = XMMatrixInverse(&det,  matViewCameraView);

    // Convert from min max representation to center extents represnetation.
    // This will make it easier to pull the points out of the transformation.
    XMFLOAT3 m_vSceneAABBMin(-10.0f, -10.0f, -10.0f);
    XMFLOAT3 m_vSceneAABBMax(10.0f,  10.0f,  10.0f);
    //XMFLOAT3 m_vSceneAABBMin(-6.0f, -6.0f, -6.0f);
    //XMFLOAT3 m_vSceneAABBMax(6.0f,  6.0f,  6.0f);
    XMVECTOR vSceneAABBMin = XMLoadFloat3(&m_vSceneAABBMin);
    XMVECTOR vSceneAABBMax = XMLoadFloat3(&m_vSceneAABBMax);
    XMVECTOR vSceneCenter = vSceneAABBMin + vSceneAABBMax;
    vSceneCenter  *= g_vHalfVector;
    XMVECTOR vSceneExtents = vSceneAABBMax - vSceneAABBMin;
    vSceneExtents *= g_vHalfVector;    

    XMVECTOR vSceneAABBPointsLightSpace[8];
    // This function simply converts the center and extents of an AABB into 8 points
    CreateAABBPoints(vSceneAABBPointsLightSpace, vSceneCenter, vSceneExtents);
    // Transform the scene AABB to Light space.
    for(int index =0; index < 8; ++index) 
    {
        vSceneAABBPointsLightSpace[index] = XMVector4Transform(vSceneAABBPointsLightSpace[index], matLightCameraView); 
    }


    FLOAT fFrustumIntervalBegin, fFrustumIntervalEnd;
    XMVECTOR vLightCameraOrthographicMin;  // light space frustrum aabb 
    XMVECTOR vLightCameraOrthographicMax;
    FLOAT fCameraNearFarRange = 20.0f;

    XMVECTOR vWorldUnitsPerTexel = g_vZero; 

    // We loop over the cascades to calculate the orthographic projection for each cascade.
    for(INT iCascadeIndex=0; iCascadeIndex < m_CascadeConfig.m_nCascadeLevels; ++iCascadeIndex) 
    {
        // Calculate the interval of the View Frustum that this cascade covers. We measure the interval 
        // the cascade covers as a Min and Max distance along the Z Axis.

        // Because we want to fit the orthogrpahic projection tightly around the Cascade, we set the Mimiumum cascade 
        // value to the previous Frustum end Interval
        if(iCascadeIndex==0) 
            fFrustumIntervalBegin = 0.0f;
        else 
            fFrustumIntervalBegin = (FLOAT)m_CascadeConfig.m_iCascadePartitionsZeroToOne[ iCascadeIndex - 1 ];


        // Scale the intervals between 0 and 1. They are now percentages that we can scale with.
        fFrustumIntervalEnd = (FLOAT)m_CascadeConfig.m_iCascadePartitionsZeroToOne[ iCascadeIndex ];        
        fFrustumIntervalBegin/= (FLOAT)m_CascadeConfig.m_iCascadePartitionsMax;
        fFrustumIntervalEnd/= (FLOAT)m_CascadeConfig.m_iCascadePartitionsMax;
        fFrustumIntervalBegin = fFrustumIntervalBegin * fCameraNearFarRange;
        fFrustumIntervalEnd = fFrustumIntervalEnd * fCameraNearFarRange;
        XMVECTOR vFrustumPoints[8];

        // This function takes the began and end intervals along with the projection matrix and returns the 8
        // points that repreresent the cascade Interval
        CreateFrustumPointsFromCascadeInterval(fFrustumIntervalBegin, fFrustumIntervalEnd, 
            matViewCameraProjection, vFrustumPoints);

        vLightCameraOrthographicMin = g_vFLTMAX;
        vLightCameraOrthographicMax = g_vFLTMIN;

        XMVECTOR vTempTranslatedCornerPoint;
        // This next section of code calculates the min and max values for the orthographic projection.
        for(int icpIndex=0; icpIndex < 8; ++icpIndex) 
        {
            // Transform the frustum from camera view space to world space.
            vFrustumPoints[icpIndex] = XMVector4Transform (vFrustumPoints[icpIndex], matInverseViewCamera);
            // Transform the point from world space to Light Camera Space.
            vTempTranslatedCornerPoint = XMVector4Transform (vFrustumPoints[icpIndex], matLightCameraView);
            // Find the closest point.
            vLightCameraOrthographicMin = XMVectorMin (vTempTranslatedCornerPoint, vLightCameraOrthographicMin);
            vLightCameraOrthographicMax = XMVectorMax (vTempTranslatedCornerPoint, vLightCameraOrthographicMax);
        }


        // This code removes the shimmering effect along the edges of shadows due to
        // the light changing to fit the camera.

        // We calculate a looser bound based on the size of the PCF blur.  This ensures us that we're 
        // sampling within the correct map.
        float fScaleDuetoBlureAMT = ((float)(m_CascadeConfig.m_iShadowBlurSize * 2 + 1) 
            /(float)m_CascadeConfig.m_iBufferSize);
        XMVECTORF32 vScaleDuetoBlureAMT = { fScaleDuetoBlureAMT, fScaleDuetoBlureAMT, 0.0f, 0.0f };


        float fNormalizeByBufferSize = (1.0f / (float)m_CascadeConfig.m_iBufferSize);
        XMVECTOR vNormalizeByBufferSize = XMVectorSet(fNormalizeByBufferSize, fNormalizeByBufferSize, 0.0f, 0.0f);

        // We calculate the offsets as a percentage of the bound.
        XMVECTOR vBoarderOffset = vLightCameraOrthographicMax - vLightCameraOrthographicMin;
        vBoarderOffset *= g_vHalfVector;
        vBoarderOffset *= vScaleDuetoBlureAMT;
        vLightCameraOrthographicMax += vBoarderOffset;
        vLightCameraOrthographicMin -= vBoarderOffset;

        // The world units per texel are used to snap  the orthographic projection
        // to texel sized increments.  
        // Because we're fitting tighly to the cascades, the shimmering shadow edges will still be present when the 
        // camera rotates.  However, when zooming in or strafing the shadow edge will not shimmer.
        vWorldUnitsPerTexel = vLightCameraOrthographicMax - vLightCameraOrthographicMin;
        vWorldUnitsPerTexel *= vNormalizeByBufferSize;



        if(m_CascadeConfig.m_bMoveLightTexelSize) 
        {

            // We snape the camera to 1 pixel increments so that moving the camera does not cause the shadows to jitter.
            // This is a matter of integer dividing by the world space size of a texel
            vLightCameraOrthographicMin /= vWorldUnitsPerTexel;
            vLightCameraOrthographicMin = XMVectorFloor(vLightCameraOrthographicMin);
            vLightCameraOrthographicMin *= vWorldUnitsPerTexel;

            vLightCameraOrthographicMax /= vWorldUnitsPerTexel;
            vLightCameraOrthographicMax = XMVectorFloor(vLightCameraOrthographicMax);
            vLightCameraOrthographicMax *= vWorldUnitsPerTexel;

        }

        //These are the unconfigured near and far plane values.  They are purposly awful to show 
        // how important calculating accurate near and far planes is.
        FLOAT fNearPlane = 0.0f;
        FLOAT fFarPlane = 20.0f;

        XMVECTOR vLightSpaceSceneAABBminValue = g_vFLTMAX;  // world space scene aabb 
        XMVECTOR vLightSpaceSceneAABBmaxValue = g_vFLTMIN;       
        // We calculate the min and max vectors of the scene in light space. The min and max "Z" values of the  
        // light space AABB can be used for the near and far plane. This is easier than intersecting the scene with the AABB
        // and in some cases provides similar results.
        for(int index=0; index< 8; ++index) 
        {
            vLightSpaceSceneAABBminValue = XMVectorMin(vSceneAABBPointsLightSpace[index], vLightSpaceSceneAABBminValue);
            vLightSpaceSceneAABBmaxValue = XMVectorMax(vSceneAABBPointsLightSpace[index], vLightSpaceSceneAABBmaxValue);
        }

        // The min and max z values are the near and far planes.
        fNearPlane = XMVectorGetZ(vLightSpaceSceneAABBminValue);
        fFarPlane = XMVectorGetZ(vLightSpaceSceneAABBmaxValue);

        //// By intersecting the light frustum with the scene AABB we can get a tighter bound on the near and far plane.
        //ComputeNearAndFar(fNearPlane, fFarPlane, vLightCameraOrthographicMin, 
        //    vLightCameraOrthographicMax, vSceneAABBPointsLightSpace);


        // Craete the orthographic projection for this cascade.
        XMMATRIX mShadowProj = XMMatrixOrthographicOffCenterLH(
            XMVectorGetX(vLightCameraOrthographicMin), 
            XMVectorGetX(vLightCameraOrthographicMax), 
            XMVectorGetY(vLightCameraOrthographicMin), 
            XMVectorGetY(vLightCameraOrthographicMax), 
            fNearPlane, fFarPlane);
        XMStoreFloat4x4(&m_mShadowProj[iCascadeIndex], mShadowProj);

        m_CascadeConfig.m_fCascadePartitionsFrustum[ iCascadeIndex ] = fFrustumIntervalEnd;
    }

    XMStoreFloat4x4(&m_mShadowView, pLight->GetViewMtx());

}

HRESULT WECascadeManager::UpdataCascadeConstantBuffer()
{
    HRESULT hr;

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V_RETURN(m_pImmediateContext->Map(m_pCascadeConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
    CascadeShadowBufferType* pcb = (CascadeShadowBufferType*)MappedResource.pData;

    // This is a floating point number that is used as the percentage to blur between maps.    
    pcb->m_fCascadeBlendArea = m_CascadeConfig.m_fBlurBetweenCascadesAmount;
    pcb->m_fTexelSize = 1.0f / (float)m_CascadeConfig.m_iBufferSize; 
    pcb->m_fNativeTexelSizeInX = pcb->m_fTexelSize / m_CascadeConfig.m_nCascadeLevels;


    XMMATRIX mTextureScale = XMMatrixScaling(0.5f, -0.5f, 1.0f);
    XMMATRIX mTextureTranslation = XMMatrixTranslation(0.5f, 0.5f, 0.0f);
    XMMATRIX scaleToTile = XMMatrixScaling(1.0f / (float)m_CascadeConfig.m_nCascadeLevels, 1.0f, 1.0f);
    for(int index=0; index < m_CascadeConfig.m_nCascadeLevels; ++index) 
    {
        XMMATRIX mShadowTexture = XMLoadFloat4x4(m_mShadowProj+index) * mTextureScale * mTextureTranslation;
        pcb->m_vCascadeScale[index].x = mShadowTexture._11;
        pcb->m_vCascadeScale[index].y = mShadowTexture._22;
        pcb->m_vCascadeScale[index].z = mShadowTexture._33;
        pcb->m_vCascadeScale[index].w = 1;

        pcb->m_vCascadeOffset[index].x = mShadowTexture._41;
        pcb->m_vCascadeOffset[index].y = mShadowTexture._42;
        pcb->m_vCascadeOffset[index].z = mShadowTexture._43;
        pcb->m_vCascadeOffset[index].w = 0;
    }


    // Copy intervals for the depth interval selection method.
    memcpy(pcb->m_fCascadeFrustumsEyeSpaceDepths, 
        m_CascadeConfig.m_fCascadePartitionsFrustum, MAX_CASCADES*4);


    // The border padding values keep the pixel shader from reading the borders during PCF filtering.
    pcb->m_fMaxBorderPadding = (float)(m_CascadeConfig.m_iBufferSize  - 1.0f) / (float)m_CascadeConfig.m_iBufferSize;
    pcb->m_fMinBorderPadding = (float)(1.0f) / (float)m_CascadeConfig.m_iBufferSize;

    XMVECTOR ep = XMLoadFloat3(&WE::Light()->GetEyePosition());
    XMVECTOR lp = XMLoadFloat3(&WE::Light()->GetLookatPosition());
    XMVECTOR dir = XMVector3Normalize(ep - lp);
    XMStoreFloat4(&pcb->m_vLightDir, XMVectorSetW(dir, 1.0f));
    pcb->m_nCascadeLevels = m_CascadeConfig.m_nCascadeLevels;
    pcb->m_iVisualizeCascades = false;

    m_pImmediateContext->Unmap(m_pCascadeConstantBuffer, 0);

    return S_OK;
}

void WECascadeManager::Render(vector<WEMesh*>* pMeshs, vector<WESkeletonMesh*>* pSkeletonMeshs)
{
    ComputeMatrices();

    // Process shadow map.

    // render cascade shadow map
    m_pImmediateContext->RSSetState(m_prsShadow);

    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // render cascade shadow map
    for(INT currentCascade=0; currentCascade < m_CascadeConfig.m_nCascadeLevels; ++currentCascade) 
    {
        m_pImmediateContext->ClearDepthStencilView(m_pTemporaryShadowDepthBufferDSV, D3D11_CLEAR_DEPTH, 1.0, 0);
        m_pImmediateContext->OMSetRenderTargets(1, &m_pCascadedShadowMapVarianceRTVArrayAll[currentCascade], m_pTemporaryShadowDepthBufferDSV);
        m_pImmediateContext->RSSetViewports(1, &m_RenderOneTileVP);

        XMMATRIX mWorld = XMMatrixIdentity();
        XMMATRIX mView = XMLoadFloat4x4(&m_mShadowView);
        XMMATRIX mProjection = XMLoadFloat4x4(m_mShadowProj + currentCascade);
        for (vector<WEMesh*>::iterator it = pMeshs->begin(); it != pMeshs->end(); it++)
        {
            m_pMeshRenderer->DrawCascade(*it, mWorld, mView, mProjection);
        }
        for (vector<WESkeletonMesh*>::iterator it = pSkeletonMeshs->begin();
            it != pSkeletonMeshs->end(); it++)
        {
            m_pSkeletonMeshRenderer->DrawCascade(*it, mWorld, mView, mProjection);
        }
    }

    // Process blur.
    ID3D11DepthStencilView *dsNullview = NULL;
    m_pImmediateContext->IASetInputLayout(NULL);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pImmediateContext->VSSetShader(m_pvsQuadBlur, NULL, 0);
    m_pImmediateContext->PSSetSamplers(5, 1, &m_pSamShadowPoint);
    ID3D11ShaderResourceView *srvNull = NULL;

    if (m_CascadeConfig.m_iShadowBlurSize > 1) 
    {
        INT iKernelShader = (m_CascadeConfig.m_iShadowBlurSize / 2 - 1); //3 goes to 0, 5 goes to 1 
        for (int iCurrentCascade=0; iCurrentCascade < m_CascadeConfig.m_nCascadeLevels; ++iCurrentCascade) 
        {
            m_pImmediateContext->PSSetShaderResources(5, 1, &srvNull);
            m_pImmediateContext->OMSetRenderTargets(1, &m_pCascadedShadowMapTempBlurRTV, dsNullview);
            m_pImmediateContext->PSSetShaderResources(5, 1, &m_pCascadedShadowMapVarianceSRVArrayAll[iCurrentCascade]);
            m_pImmediateContext->PSSetShader(m_ppsQuadBlurX[iKernelShader], NULL, 0);
            m_pImmediateContext->Draw(4, 0);

            m_pImmediateContext->PSSetShaderResources(5, 1, &srvNull);
            m_pImmediateContext->OMSetRenderTargets(1, &m_pCascadedShadowMapVarianceRTVArrayAll[iCurrentCascade], dsNullview);
            m_pImmediateContext->PSSetShaderResources(5, 1, &m_pCascadedShadowMapTempBlurSRV);
            m_pImmediateContext->PSSetShader(m_ppsQuadBlurY[iKernelShader], NULL, 0);
            m_pImmediateContext->Draw(4, 0);

        }
    }

    // Process scenes.
    // Clear the render target buffers.
    float color[4] = {0,0,0,1};
    for (int i=0; i<TARGET_COUNT; i++)
    {
        m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetViewArray[i], color);
    }

    // Clear the depth buffer.
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Bind the render target view array and depth stencil buffer to the output render pipeline.
    m_pImmediateContext->OMSetRenderTargets(TARGET_COUNT, m_pRenderTargetViewArray, m_pDepthStencilView);

    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_pImmediateContext->RSSetViewports(1, &m_pViewport);

    m_pImmediateContext->RSSetState(m_prsScene);

    // Set constant buffer.
    UpdataCascadeConstantBuffer();
    //m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pCascadeConstantBuffer);
    m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pCascadeConstantBuffer);

    // Set samplers and resouce for depth map.
    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamLinear);
    m_pImmediateContext->PSSetSamplers(5, 1, &m_pSamShadowAnisotropic16);

    m_pImmediateContext->PSSetShaderResources(5,1, &m_pCascadedShadowMapVarianceSRVArraySingle);

    // render scenes with shadow
    XMMATRIX mWorld = XMMatrixIdentity();
    XMMATRIX mView = WE::Camera()->GetViewMtx();
    XMMATRIX mProjection = WE::Camera()->GetProjMtx();
    XMMATRIX mShadow = XMLoadFloat4x4(&m_mShadowView);

    for (vector<WEMesh*>::iterator it = pMeshs->begin(); it != pMeshs->end(); it++)
    {
        m_pMeshRenderer->DrawScene(*it, mWorld, mView, 
            mProjection, mShadow, m_CascadeConfig.m_nCascadeLevels);
    }
    for (vector<WESkeletonMesh*>::iterator it = pSkeletonMeshs->begin();
        it != pSkeletonMeshs->end(); it++)
    {
        m_pSkeletonMeshRenderer->DrawScene(*it, mWorld, mView, 
            mProjection, mShadow, m_CascadeConfig.m_nCascadeLevels);
    }

    ID3D11ShaderResourceView* nv[] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    m_pImmediateContext->PSSetShaderResources(5,8, nv); 
}