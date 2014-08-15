#include "WEFinalPass.h"


WEFinalPass::WEFinalPass()
{
    m_pSampleLinear = NULL;
    m_pSamplePoint = NULL;

    m_pPSTestSRV = NULL;
}


WEFinalPass::~WEFinalPass()
{
    Release();
}

HRESULT WEFinalPass::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    HRESULT hr;

    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pd3dImmediateContext;

    // Create shaders
    ID3DBlob* pPSBlob = NULL;
    V_RETURN( WE::CompileShaderFromFile( L"shaders\\TestSRV.hlsl", NULL, "PSMain", "ps_5_0", &pPSBlob ) );    
    V_RETURN( WE::D3DDevice()->CreatePixelShader( pPSBlob->GetBufferPointer(  ), pPSBlob->GetBufferSize(  ), NULL, &m_pPSTestSRV ) );
    SAFE_RELEASE(pPSBlob);

    // Create the texture sampler states.
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

    return S_OK;
}

void WEFinalPass::Release()
{
    SAFE_RELEASE(m_pSamplePoint);
    SAFE_RELEASE(m_pSampleLinear);

    SAFE_RELEASE(m_pPSTestSRV);
}

void WEFinalPass::TestShaderResourceView(WEFullScreenQuad* pFullScreenQuad, 
    ID3D11RenderTargetView* pRTV, ID3D11ShaderResourceView* pSRV)
{
    pFullScreenQuad->Prepare();

    m_pImmediateContext->OMSetRenderTargets(1, &pRTV, NULL);

    m_pImmediateContext->PSSetShaderResources(0, 1, &pSRV);

    m_pImmediateContext->PSSetShader(m_pPSTestSRV, 0, 0);

    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplePoint);
    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampleLinear);

    pFullScreenQuad->DrawQuad();
}