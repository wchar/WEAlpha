#include "WEHDR.h"

namespace
{
    struct BufferType
    {
        XMFLOAT4 avSampleOffsets[15];
        XMFLOAT4 avSampleWeights[15];
    };
}

/////////////////////////////
float GaussianDistribution( float x, float y, float rho );
HRESULT GetSampleOffsets_Bloom_D3D11( DWORD dwD3DTexSize,
    float afTexCoordOffset[15],
    XMFLOAT4* avColorWeight,
    float fDeviation, float fMultiplier );

//////////////////////////////
WEHDR::WEHDR()
{    
    m_pSampleLinear = NULL;
    m_pSamplePoint = NULL;

    m_pPSDownScale2x2Lum = NULL;
    m_pPSDownScale3x3 = NULL;
    for (int i = 0; i < NUM_TONEMAP_TEXTURES; i++)
    {
        m_pTexToneMapRTV[i] = NULL;
        m_pTexToneMap[i] = NULL;
        m_pTexToneMapRV[i] = NULL;
    }

    m_pPSBloom = NULL;
    m_pcbBloom = NULL;
    for (int i = 0; i < NUM_BLOOM_TEXTURES; i++)
    {
        m_pTexBloomRTV[i] = NULL;
        m_pTexBloom[i] = NULL;
        m_pTexBloomRV[i] = NULL;
    }

    m_pPSBrightPass = NULL;
    m_pTexBrightPassRTV = NULL;
    m_pTexBrightPass = NULL;
    m_pTexBrightPassRV = NULL;

    m_pPSFinalPass = NULL;
    m_pTexFinalRTV = NULL;
    m_pTexFinal = NULL;
    m_pTexFinalRV = NULL;
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
    ID3DBlob* pPSBlobBrightPass = NULL;
    ID3DBlob* pPSBlobBloom = NULL;
    ID3DBlob* pPSBlobFinal = NULL;

    V_RETURN(WE::CompileShaderFromFile(L"shaders\\HDR.hlsl", NULL, "DownScale2x2_Lum", "ps_5_0", &pPSBlob2x2));
    V_RETURN(WE::CompileShaderFromFile(L"shaders\\HDR.hlsl", NULL, "DownScale3x3", "ps_5_0", &pPSBlob3x3));
    V_RETURN(WE::CompileShaderFromFile(L"shaders\\HDR.hlsl", NULL, "DownScale3x3_BrightPass", "ps_5_0", &pPSBlobBrightPass));
    V_RETURN(WE::CompileShaderFromFile(L"shaders\\HDR.hlsl", NULL, "Bloom", "ps_5_0", &pPSBlobBloom));
    V_RETURN(WE::CompileShaderFromFile(L"shaders\\HDR.hlsl", NULL, "FinalPass", "ps_5_0", &pPSBlobFinal));

    V_RETURN(pd3dDevice->CreatePixelShader(pPSBlob2x2->GetBufferPointer(), pPSBlob2x2->GetBufferSize(), NULL, &m_pPSDownScale2x2Lum));
    V_RETURN(pd3dDevice->CreatePixelShader(pPSBlob3x3->GetBufferPointer(), pPSBlob3x3->GetBufferSize(), NULL, &m_pPSDownScale3x3));
    V_RETURN(pd3dDevice->CreatePixelShader(pPSBlobBrightPass->GetBufferPointer(), pPSBlobBrightPass->GetBufferSize(), NULL, &m_pPSBrightPass));
    V_RETURN(pd3dDevice->CreatePixelShader(pPSBlobBloom->GetBufferPointer(), pPSBlobBloom->GetBufferSize(), NULL, &m_pPSBloom));
    V_RETURN(pd3dDevice->CreatePixelShader(pPSBlobFinal->GetBufferPointer(), pPSBlobFinal->GetBufferSize(), NULL, &m_pPSFinalPass));

    SAFE_RELEASE(pPSBlob2x2);
    SAFE_RELEASE(pPSBlob3x3);
    SAFE_RELEASE(pPSBlobBrightPass);    
    SAFE_RELEASE(pPSBlobBloom);    
    SAFE_RELEASE(pPSBlobFinal);

    // Create the texture sampler state.
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
    V_RETURN(WE::D3DDevice()->CreateSamplerState(&samplerDesc, &m_pSamplePoint));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    V_RETURN(WE::D3DDevice()->CreateSamplerState(&samplerDesc, &m_pSampleLinear)); 

    // Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(BufferType);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    V_RETURN(pd3dDevice->CreateBuffer(&bufferDesc, NULL, &m_pcbBloom));

    //
    V_RETURN(CreateViews());

    return S_OK;
}

void WEHDR::Release()
{
    SAFE_RELEASE(m_pPSDownScale2x2Lum);
    SAFE_RELEASE(m_pPSDownScale3x3);
    SAFE_RELEASE(m_pPSBrightPass);
    SAFE_RELEASE(m_pPSFinalPass);
    SAFE_RELEASE(m_pPSBloom);
    SAFE_RELEASE(m_pcbBloom);

    SAFE_RELEASE(m_pSamplePoint);
    SAFE_RELEASE(m_pSampleLinear);

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

    UINT width, height;
    WE::D3D().GetScreen(&width, &height);

    // Create the temporary blooming effect textures for PS path and buffers for CS path
    for( int i = 0; i < NUM_BLOOM_TEXTURES; i++ )
    {
        // Texture for blooming effect in PS path
        D3D11_TEXTURE2D_DESC bmdesc;
        ZeroMemory( &bmdesc, sizeof( D3D11_TEXTURE2D_DESC ) );
        bmdesc.ArraySize = 1;
        bmdesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        bmdesc.Usage = D3D11_USAGE_DEFAULT;
        bmdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        bmdesc.Width = width / 8;
        bmdesc.Height = height / 8;
        bmdesc.MipLevels = 1;
        bmdesc.SampleDesc.Count = 1;
        V_RETURN( m_pd3dDevice->CreateTexture2D( &bmdesc, NULL, &m_pTexBloom[i] ) );

        // Create the render target view
        D3D11_RENDER_TARGET_VIEW_DESC DescRT;
        DescRT.Format = bmdesc.Format;
        DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        DescRT.Texture2D.MipSlice = 0;
        V_RETURN( m_pd3dDevice->CreateRenderTargetView( m_pTexBloom[i], &DescRT, &m_pTexBloomRTV[i] ) );
   
        // Create the shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
        DescRV.Format = bmdesc.Format;
        DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        DescRV.Texture2D.MipLevels = 1;
        DescRV.Texture2D.MostDetailedMip = 0;
        V_RETURN( m_pd3dDevice->CreateShaderResourceView( m_pTexBloom[i], &DescRV, &m_pTexBloomRV[i] ) );
    }


    // Create the final pass texture 
    D3D11_TEXTURE2D_DESC Desc;
    ZeroMemory( &Desc, sizeof( D3D11_TEXTURE2D_DESC ) );
    Desc.ArraySize = 1;
    Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    Desc.Width = width;
    Desc.Height = height;
    Desc.MipLevels = 1;
    Desc.SampleDesc.Count = 1;
    V_RETURN( m_pd3dDevice->CreateTexture2D( &Desc, NULL, &m_pTexFinal ) ); 

    // Create the render target view
    D3D11_RENDER_TARGET_VIEW_DESC DescRT;
    DescRT.Format = Desc.Format;
    DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    DescRT.Texture2D.MipSlice = 0;
    V_RETURN( m_pd3dDevice->CreateRenderTargetView( m_pTexFinal, &DescRT, &m_pTexFinalRTV ) );

    // Create the resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
    DescRV.Format = Desc.Format;
    DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    DescRV.Texture2D.MipLevels = 1;
    DescRV.Texture2D.MostDetailedMip = 0;
    V_RETURN( m_pd3dDevice->CreateShaderResourceView( m_pTexFinal, &DescRV, &m_pTexFinalRV ) );



    // Create the bright pass texture
    Desc.Width /= 8;
    Desc.Height /= 8;
    Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    V_RETURN( m_pd3dDevice->CreateTexture2D( &Desc, NULL, &m_pTexBrightPass ) );

    // Create the render target view
    DescRT.Format = Desc.Format;
    DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    DescRT.Texture2D.MipSlice = 0;
    V_RETURN( m_pd3dDevice->CreateRenderTargetView( m_pTexBrightPass, &DescRT, &m_pTexBrightPassRTV ) );

    // Create the resource view
    DescRV.Format = Desc.Format;
    DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    DescRV.Texture2D.MipLevels = 1;
    DescRV.Texture2D.MostDetailedMip = 0;
    V_RETURN( m_pd3dDevice->CreateShaderResourceView( m_pTexBrightPass, &DescRV, &m_pTexBrightPassRV ) );



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

    for (int i = 0; i < NUM_BLOOM_TEXTURES; i++)
    {
        SAFE_RELEASE(m_pTexBloomRTV[i]);
        SAFE_RELEASE(m_pTexBloom[i]);
        SAFE_RELEASE(m_pTexBloomRV[i]);
    }

    SAFE_RELEASE(m_pTexBrightPassRTV);
    SAFE_RELEASE(m_pTexBrightPass);
    SAFE_RELEASE(m_pTexBrightPassRV);

    SAFE_RELEASE(m_pTexFinalRTV);
    SAFE_RELEASE(m_pTexFinal);
    SAFE_RELEASE(m_pTexFinalRV);
}

void WEHDR::Process(WEFullScreenQuad* pFuffScreenQuad, ID3D11ShaderResourceView* pColorRV)
{
    // Set vertex shader, quad buffer, full screen layout.
    pFuffScreenQuad->Prepare();

    // Save the old viewport
    D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];    
    UINT nViewPorts = 1;
    m_pImmediateContext->RSGetViewports(&nViewPorts, vpOld); 

    ProcessLum(pColorRV);

    ProcessBright(pColorRV);

    ProcessBloom();

    // Restore the Old viewport
    m_pImmediateContext->RSSetViewports(nViewPorts, vpOld);

    ProcessFinalPass(pColorRV);
}

void WEHDR::ProcessLum(ID3D11ShaderResourceView* pColorRV)
{
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


}

void WEHDR::ProcessBright(ID3D11ShaderResourceView* pColorRV)
{
    D3D11_TEXTURE2D_DESC descDest;
    m_pTexBrightPass->GetDesc(&descDest);

    D3D11_VIEWPORT vp;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0; 
    vp.Width = (float)descDest.Width;
    vp.Height = (float)descDest.Height;
    m_pImmediateContext->RSSetViewports(1, &vp);

    m_pImmediateContext->OMSetRenderTargets( 1, &m_pTexBrightPassRTV, NULL );

    ID3D11ShaderResourceView* aRViews[ 2 ] = { pColorRV, m_pTexToneMapRV[0] };
    m_pImmediateContext->PSSetShaderResources( 0, 2, aRViews );  

    m_pImmediateContext->PSSetSamplers( 0, 1, &m_pSamplePoint );

    m_pImmediateContext->PSSetShader(m_pPSBrightPass, 0, 0);
    
    m_pImmediateContext->Draw(6, 0);
}

void WEHDR::ProcessBloom()
{
    HRESULT hr;

    UINT width, height;
    WE::D3D().GetScreen(&width, &height);
    width /= 8;
    height /= 8;

    ID3D11ShaderResourceView* pFromRV = m_pTexBrightPassRV;
    ID3D11ShaderResourceView* pAuxRV = m_pTexBloomRV[1];
    ID3D11RenderTargetView* pAuxRTV = m_pTexBloomRTV[1];
    ID3D11RenderTargetView* pToRTV = m_pTexBloomRTV[0];

    // Horizontal Blur
    D3D11_MAPPED_SUBRESOURCE MappedResource;            
    V( m_pImmediateContext->Map( m_pcbBloom, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    BufferType* pcbBloom = ( BufferType* )MappedResource.pData;
    XMFLOAT4* avSampleOffsets = pcbBloom->avSampleOffsets;
    XMFLOAT4* avSampleWeights = pcbBloom->avSampleWeights;

    float afSampleOffsets[15];
    hr = GetSampleOffsets_Bloom_D3D11( width, afSampleOffsets, avSampleWeights, 3.0f, 1.25f );
    for (int i = 0; i < 15; i++)
    { 
        avSampleOffsets[i] = XMFLOAT4( afSampleOffsets[i], 0.0f, 0.0f, 0.0f );
    }
    m_pImmediateContext->Unmap( m_pcbBloom, 0 );

    ID3D11Buffer* ppCB[1] = { m_pcbBloom };
    m_pImmediateContext->PSSetConstantBuffers( 0, 1, ppCB );

    ID3D11RenderTargetView* aRTViews[ 1 ] = { pAuxRTV };
    m_pImmediateContext->OMSetRenderTargets( 1, aRTViews, NULL );

    ID3D11ShaderResourceView* pViews[1] = {pFromRV};
    m_pImmediateContext->PSSetShaderResources( 0, 1, pViews );
    ID3D11SamplerState* aSamplers[] = { m_pSamplePoint };
    m_pImmediateContext->PSSetSamplers( 0, 1, aSamplers );
    
    m_pImmediateContext->PSSetShader(m_pPSBloom, 0, 0);
    // Draw quad.   
    m_pImmediateContext->Draw(6, 0);

    ID3D11ShaderResourceView* pViewsNull[4] = {0,0,0,0};
    m_pImmediateContext->PSSetShaderResources( 0, 4, pViewsNull );

    // Vertical Blur
    V( m_pImmediateContext->Map( m_pcbBloom, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    pcbBloom = ( BufferType* )MappedResource.pData;
    avSampleOffsets = pcbBloom->avSampleOffsets;
    avSampleWeights = pcbBloom->avSampleWeights;
    hr = GetSampleOffsets_Bloom_D3D11( height, afSampleOffsets, avSampleWeights, 3.0f, 1.25f );
    for (int i = 0; i < 15; i++)
    {
        avSampleOffsets[i] = XMFLOAT4( 0.0f, afSampleOffsets[i], 0.0f, 0.0f );
    }
    m_pImmediateContext->Unmap( m_pcbBloom, 0 );

    ppCB[0] = m_pcbBloom;
    m_pImmediateContext->PSSetConstantBuffers( 0, 1, ppCB );

    aRTViews[ 0 ] = pToRTV;
    m_pImmediateContext->OMSetRenderTargets( 1, aRTViews, NULL );

    pViews[0] = pAuxRV;
    m_pImmediateContext->PSSetShaderResources( 0, 1, pViews );

    m_pImmediateContext->PSSetShader(m_pPSBloom, 0, 0);
    // Draw quad.   
    m_pImmediateContext->Draw(6, 0);

    m_pImmediateContext->PSSetShaderResources( 0, 4, pViewsNull );

    ID3D11Buffer* ppCBNull[1] = { NULL };
    m_pImmediateContext->PSSetConstantBuffers( 0, 1, ppCBNull );

}

void WEHDR::ProcessFinalPass(ID3D11ShaderResourceView* pColorRV)
{
    m_pImmediateContext->OMSetRenderTargets( 1, &m_pTexFinalRTV, NULL ); 

    ID3D11ShaderResourceView* aRViews[ 3 ] = { pColorRV, m_pTexToneMapRV[0], m_pTexBloomRV[0] };
    m_pImmediateContext->PSSetShaderResources( 0, 3, aRViews );

    ID3D11SamplerState* aSamplers[] = { m_pSamplePoint, m_pSampleLinear };
    m_pImmediateContext->PSSetSamplers( 0, 2, aSamplers );

    m_pImmediateContext->PSSetShader(m_pPSFinalPass, 0, 0);

    // Draw quad.   
    m_pImmediateContext->Draw(6, 0);

}


float GaussianDistribution( float x, float y, float rho )
{
    float g = 1.0f / sqrtf( 2.0f * XM_PI * rho * rho );
    g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

    return g;
}

HRESULT GetSampleOffsets_Bloom_D3D11( DWORD dwD3DTexSize,
                                     float afTexCoordOffset[15],
                                     XMFLOAT4* avColorWeight,
                                     float fDeviation, float fMultiplier )
{
    int i = 0;
    float tu = 1.0f / ( float )dwD3DTexSize;

    // Fill the center texel
    float weight = 1.0f * GaussianDistribution( 0, 0, fDeviation );
    avColorWeight[7] = XMFLOAT4( weight, weight, weight, 1.0f );

    afTexCoordOffset[7] = 0.0f;

    // Fill one side
    for( i = 1; i < 8; i++ )
    {
        weight = fMultiplier * GaussianDistribution( ( float )i, 0, fDeviation );
        afTexCoordOffset[7-i] = -i * tu;

        avColorWeight[7-i] = XMFLOAT4( weight, weight, weight, 1.0f );
    }

    // Copy to the other side
    for( i = 8; i < 15; i++ )
    {
        avColorWeight[i] = avColorWeight[14 - i];
        afTexCoordOffset[i] = -afTexCoordOffset[14 - i];
    }

    return S_OK;
}
