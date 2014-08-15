#include "WESkeletonMeshRenderer.h"
#include "WEBaseCamera.h"

namespace
{
    struct VSBufferType
    {
        XMMATRIX mWorldViewProj;
        XMMATRIX mWorldView;
        XMMATRIX mWorld;
        XMMATRIX mShadow;
        XMMATRIX mConstBoneWorld[MAX_JOINT];
    };

    struct PSBufferType
    {
        XMFLOAT4 vDiffuse;
        XMFLOAT4 vAmbient;
        XMFLOAT4 vEmissive;
        XMFLOAT4 vSpecular;
    };

}

WESkeletonMeshRenderer::WESkeletonMeshRenderer()
{
    m_pd3dDevice = NULL;
    m_pImmediateContext = NULL;

    m_pVSBuffer = NULL;
    m_pPSBuffer = NULL;

    m_pVertexLayoutMesh = NULL;

    m_pvsRenderVarianceShadow = NULL;   
    m_ppsRenderVarianceShadow = NULL;
    m_pvsRenderVarianceShadowBlob = NULL;
    m_ppsRenderVarianceShadowBlob = NULL;

    m_pvsRenderScene = NULL;
    m_pvsRenderSceneBlob = NULL;
    for( INT index=0; index < MAX_CASCADES; ++index ) 
    {    
        m_ppsRenderSceneAllShaders[index] = NULL;
        m_ppsRenderSceneAllShadersBlob[index] = NULL;
    }
}


WESkeletonMeshRenderer::~WESkeletonMeshRenderer()
{
    Release();
}

HRESULT WESkeletonMeshRenderer::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext)
{
    HRESULT hr = S_OK;

    V_RETURN(WEMeshRenderer::Create(pd3dDevice, pImmediateContext));
    
    SAFE_RELEASE(m_pVertexLayoutMesh);
    SAFE_RELEASE(m_pvsRenderSceneBlob);
    SAFE_RELEASE(m_pvsRenderScene);
    SAFE_RELEASE(m_pVSBuffer);

    // VS scene
    V_RETURN( WE::CompileShaderFromFile( L"shaders\\SkeletonMeshVS.hlsl", NULL, "VSMain", "vs_4_0", &m_pvsRenderSceneBlob ) );    
    V_RETURN( pd3dDevice->CreateVertexShader( m_pvsRenderSceneBlob->GetBufferPointer(), 
        m_pvsRenderSceneBlob->GetBufferSize(), NULL, &m_pvsRenderScene) );

    // Vertex layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"WEIGHTS",  0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONES",    0, DXGI_FORMAT_R8G8B8A8_UINT,   0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    V_RETURN( pd3dDevice->CreateInputLayout( 
        layout, ARRAYSIZE( layout ), 
        m_pvsRenderSceneBlob->GetBufferPointer(),
        m_pvsRenderSceneBlob->GetBufferSize(), 
        &m_pVertexLayoutMesh ) );

    // Create the constant buffers
    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;

    Desc.ByteWidth = sizeof( VSBufferType );
    V_RETURN( m_pd3dDevice->CreateBuffer( &Desc, NULL, &m_pVSBuffer ) );

    return hr;
}

void WESkeletonMeshRenderer::Release()
{
    SAFE_RELEASE(m_pVSBuffer);
    SAFE_RELEASE(m_pPSBuffer);

    SAFE_RELEASE(m_pVertexLayoutMesh);

    SAFE_RELEASE( m_pvsRenderVarianceShadow );
    SAFE_RELEASE( m_ppsRenderVarianceShadow );
    SAFE_RELEASE( m_pvsRenderVarianceShadowBlob );
    SAFE_RELEASE( m_ppsRenderVarianceShadowBlob );

    SAFE_RELEASE(m_pvsRenderScene);
    SAFE_RELEASE(m_pvsRenderSceneBlob);

    for( INT index=0; index < MAX_CASCADES; ++index ) 
    {    
        SAFE_RELEASE(m_ppsRenderSceneAllShaders[index]);
        SAFE_RELEASE(m_ppsRenderSceneAllShadersBlob[index]);
    }
}

void WESkeletonMeshRenderer::DrawCascade(
    WESkeletonMesh* pMesh, 
    CXMMATRIX mWorld, 
    CXMMATRIX mView, 
    CXMMATRIX mProjection)
{
    HRESULT hr;

    // Set input layout.
    m_pImmediateContext->IASetInputLayout(m_pVertexLayoutMesh);

    // Set shaders.
    m_pImmediateContext->VSSetShader( m_pvsRenderVarianceShadow, NULL, 0 );
    m_pImmediateContext->PSSetShader( m_ppsRenderVarianceShadow, NULL, 0 );

    // Set constant buffer per obj.
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V( m_pImmediateContext->Map( m_pVSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    VSBufferType* pVSBuffer = ( VSBufferType* )MappedResource.pData;
    pVSBuffer->mWorldViewProj = XMMatrixTranspose( mWorld * mView * mProjection );
    m_pImmediateContext->Unmap( m_pVSBuffer, 0 );

    m_pImmediateContext->VSSetConstantBuffers( 0, 1, &m_pVSBuffer );

    // Set vertex buffer.
    UINT Strides[1];
    UINT Offsets[1];
    ID3D11Buffer* pVB[1];
    pVB[0] = pMesh->GetVB();
    Strides[0] = pMesh->GetVBStride( ); 
    Offsets[0] = 0;
    m_pImmediateContext->IASetVertexBuffers( 0, 1, pVB, Strides, Offsets );

    // Set index buffer.
    m_pImmediateContext->IASetIndexBuffer( pMesh->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

    // Draw mesh.
    vector<MESH_SUBSET*>* pSubsets = pMesh->GetSubsets();
    MESH_SUBSET* pItem;
    for ( UINT subset = 0; subset < pSubsets->size(); subset++ )
    {
        pItem = pSubsets->at( subset) ;

        // Draw subset.
        m_pImmediateContext->DrawIndexed( pItem->IndexCount, pItem->IndexStart, pItem->VertexStart );
    }
}

void WESkeletonMeshRenderer::DrawScene(
    WESkeletonMesh* pMesh,
    CXMMATRIX mWorld, 
    CXMMATRIX mView, 
    CXMMATRIX mProjection, 
    CXMMATRIX mShadowView, 
    UINT uCascadeLevel)
{
    HRESULT hr;
    // Set input layout.
    m_pImmediateContext->IASetInputLayout(m_pVertexLayoutMesh);

    // Set vertex shader.
    m_pImmediateContext->VSSetShader(m_pvsRenderScene, NULL, 0);

    // Set constant buffer per obj.

    UINT uJointCount = pMesh->GetJointCount();
    XMMATRIX* matrixArray = pMesh->GetMatrixArray();
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V( m_pImmediateContext->Map( m_pVSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    VSBufferType* pVSBuffer = ( VSBufferType* )MappedResource.pData;
    pVSBuffer->mWorldViewProj = XMMatrixTranspose( mWorld * mView * mProjection );
    pVSBuffer->mWorldView = XMMatrixTranspose( mWorld * mView );
    pVSBuffer->mWorld = XMMatrixTranspose( mWorld );
    pVSBuffer->mShadow = XMMatrixTranspose( mShadowView );
    for( int j = 0; j < uJointCount; ++j )
    {
        pVSBuffer->mConstBoneWorld[j] = XMMatrixTranspose(matrixArray[j]);
    }
    m_pImmediateContext->Unmap( m_pVSBuffer, 0 );

    m_pImmediateContext->VSSetConstantBuffers( 0, 1, &m_pVSBuffer );

    // Set vertex buffer.
    UINT Strides[1];
    UINT Offsets[1];
    ID3D11Buffer* pVB[1];
    pVB[0] = pMesh->GetVB();
    Strides[0] = pMesh->GetVBStride( ); 
    Offsets[0] = 0;
    m_pImmediateContext->IASetVertexBuffers( 0, 1, pVB, Strides, Offsets );

    // Set index buffer.
    m_pImmediateContext->IASetIndexBuffer( pMesh->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

    // Draw subsets.
    vector<WEMaterial*>* pMaterials = pMesh->GetMaterials();
    vector<MESH_SUBSET*>* pSubsets = pMesh->GetSubsets();
    MESH_SUBSET* pItem;
    for ( UINT subset = 0; subset < pSubsets->size(); subset++ )
    {
        pItem = pSubsets->at( subset) ;
        WEMaterial* pMaterial = pMaterials->at( pItem->MaterialID );

        // Set PS buffer.
        V( m_pImmediateContext->Map( m_pPSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
        PSBufferType* pPSBuffer = ( PSBufferType* )MappedResource.pData;
        pPSBuffer->vAmbient = pMaterial->m_vAmbient;
        pPSBuffer->vDiffuse = pMaterial->m_vDiffuse;
        pPSBuffer->vEmissive = pMaterial->m_vEmissive;
        pPSBuffer->vSpecular = pMaterial->m_vSpecular;
        m_pImmediateContext->Unmap( m_pPSBuffer, 0 );

        m_pImmediateContext->PSSetConstantBuffers( 1, 1, &m_pPSBuffer );

        // Set PS resource views.
        ID3D11ShaderResourceView* pArraySRV[] = 
        {
            pMaterial->GetDiffuseTexture() ? pMaterial->GetDiffuseTexture()->GetSRV() : NULL,
            pMaterial->GetEmissiveTexture() ? pMaterial->GetEmissiveTexture()->GetSRV() : NULL,
            pMaterial->GetNormalTexture() ? pMaterial->GetNormalTexture()->GetSRV() : NULL
        };
        m_pImmediateContext->PSSetShaderResources( 0, 3, pArraySRV );

        // Set PS.
        m_pImmediateContext->PSSetShader( m_ppsRenderSceneAllShaders[uCascadeLevel-1], NULL, 0 );

        // Draw subset.
        m_pImmediateContext->DrawIndexed( pItem->IndexCount, pItem->IndexStart, pItem->VertexStart );
    }
}