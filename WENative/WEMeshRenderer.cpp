#include "WEMeshRenderer.h"
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
        XMFLOAT3 vDiffuse;
        FLOAT fAlpha;

        XMFLOAT3 vEmissive;
        FLOAT fStar;

        XMFLOAT3 vSpecular;
        FLOAT fShininess;

        XMFLOAT3 vAmbient;
        INT iMaterialFlag;
    };
}

WEMeshRenderer::WEMeshRenderer()
{
    m_pd3dDevice = NULL;
    m_pImmediateContext = NULL;
       
    m_pVSBuffer = NULL;
    m_pPSBuffer = NULL;

    m_pVertexLayoutMesh = NULL;
    m_pVertexLayoutSkeletonMesh = NULL;

    m_pvsRenderVarianceShadow = NULL;   
    m_ppsRenderVarianceShadow = NULL;
    m_pvsRenderVarianceShadowBlob = NULL;
    m_ppsRenderVarianceShadowBlob = NULL;

    m_pvsRenderScene = NULL;
    m_pvsRenderSceneBlob = NULL;
 
    m_pvsRenderSkeletonScene = NULL;
    m_pvsRenderSkeletonSceneBlob = NULL;    
    m_pvsRenderSkeletonVarianceShadow = NULL;     
    m_pvsRenderSkeletonVarianceShadowBlob = NULL;

    for( INT index=0; index < MAX_CASCADES; ++index ) {    
        for (int iX = 0; iX < 2; ++iX ) {
            for (int iX2 = 0; iX2 < 2; ++iX2 ) {
                m_ppsRenderSceneAllShaders[index][iX][iX2]= NULL;
                m_ppsRenderSceneAllShadersBlob[index][iX][iX2] = NULL;
            }
        }
    }
}

WEMeshRenderer::~WEMeshRenderer()
{
    Release();
}

HRESULT WEMeshRenderer::Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext)
{
    HRESULT hr = S_OK;

    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pImmediateContext;

    // VS shadow
    V_RETURN( WE::CompileShaderFromFile( L"shaders\\MeshCascade.hlsl", NULL, "VSMain", "vs_4_0", &m_pvsRenderVarianceShadowBlob ) );  
    V_RETURN( pd3dDevice->CreateVertexShader(m_pvsRenderVarianceShadowBlob->GetBufferPointer(), 
        m_pvsRenderVarianceShadowBlob->GetBufferSize(), 
        NULL, &m_pvsRenderVarianceShadow) );

    V_RETURN( WE::CompileShaderFromFile( L"shaders\\MeshCascade.hlsl", NULL, "VSMain_SKIN", "vs_4_0", &m_pvsRenderSkeletonVarianceShadowBlob ) );
    V_RETURN( pd3dDevice->CreateVertexShader(m_pvsRenderSkeletonVarianceShadowBlob->GetBufferPointer(), 
        m_pvsRenderSkeletonVarianceShadowBlob->GetBufferSize(), 
        NULL, &m_pvsRenderSkeletonVarianceShadow) );

    // PS shadow
        V_RETURN( WE::CompileShaderFromFile( L"shaders\\MeshCascade.hlsl", NULL, "PSMain", "ps_4_0", &m_ppsRenderVarianceShadowBlob ) );
        V_RETURN( pd3dDevice->CreatePixelShader(m_ppsRenderVarianceShadowBlob->GetBufferPointer(), 
            m_ppsRenderVarianceShadowBlob->GetBufferSize(), 
            NULL, &m_ppsRenderVarianceShadow) );

    // VS scene
    V_RETURN( WE::CompileShaderFromFile( L"shaders\\MeshVS.hlsl", NULL, "VSMain", "vs_4_0", &m_pvsRenderSceneBlob ) );    
    V_RETURN( pd3dDevice->CreateVertexShader( m_pvsRenderSceneBlob->GetBufferPointer(), 
        m_pvsRenderSceneBlob->GetBufferSize(), NULL, &m_pvsRenderScene) );

    V_RETURN( WE::CompileShaderFromFile( L"shaders\\SkeletonMeshVS.hlsl", NULL, "VSMain", "vs_4_0", &m_pvsRenderSkeletonSceneBlob ) );    
    V_RETURN( pd3dDevice->CreateVertexShader( m_pvsRenderSkeletonSceneBlob->GetBufferPointer(), 
        m_pvsRenderSkeletonSceneBlob->GetBufferSize(), NULL, &m_pvsRenderSkeletonScene) );

    // PS scene
    char cCascadeDefinition[32];       
    char cBlendDefinition[32];
    char cIntervalDefinition[32];

    D3D_SHADER_MACRO defines[] = 
    {
        "CASCADE_COUNT_FLAG", "1",
        "BLEND_BETWEEN_CASCADE_LAYERS_FLAG", "0",
        "SELECT_CASCADE_BY_INTERVAL_FLAG", "0",
        NULL, NULL
    };
    for (int i = 0; i < MAX_CASCADES; i++)
    {
        for( INT iBlendIndex=0; iBlendIndex < 2; ++iBlendIndex ) 
        {
            for( INT iIntervalIndex=0; iIntervalIndex < 2; ++iIntervalIndex ) 
            {            
                sprintf_s( cCascadeDefinition, "%d", i + 1 );                             
                sprintf_s( cBlendDefinition, "%d", iBlendIndex );                     
                sprintf_s( cIntervalDefinition, "%d", iIntervalIndex );            
                
                defines[0].Definition = cCascadeDefinition;  
                defines[1].Definition = cBlendDefinition;
                defines[2].Definition = cIntervalDefinition;  

                V_RETURN( WE::CompileShaderFromFile( L"shaders\\MeshPS.hlsl", 
                    defines, "PSMain", "ps_4_0", &
                    m_ppsRenderSceneAllShadersBlob[i]
                [iBlendIndex]
                [iIntervalIndex]) );
                V_RETURN( pd3dDevice->CreatePixelShader( 
                    m_ppsRenderSceneAllShadersBlob[i]
                [iBlendIndex]
                [iIntervalIndex]->GetBufferPointer(),
                    m_ppsRenderSceneAllShadersBlob[i]
                [iBlendIndex]
                [iIntervalIndex]->GetBufferSize(), 
                    NULL, &m_ppsRenderSceneAllShaders[i]
                [iBlendIndex]
                [iIntervalIndex] ) );
            }
        }
    }

    // Vertex layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    V_RETURN( pd3dDevice->CreateInputLayout( 
        layout, ARRAYSIZE( layout ), 
        m_pvsRenderSceneBlob->GetBufferPointer(),
        m_pvsRenderSceneBlob->GetBufferSize(), 
        &m_pVertexLayoutMesh ) );
    
    D3D11_INPUT_ELEMENT_DESC layout2[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"WEIGHTS",  0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONES",    0, DXGI_FORMAT_R8G8B8A8_UINT,   0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    V_RETURN( pd3dDevice->CreateInputLayout( 
        layout2, ARRAYSIZE( layout2 ), 
        m_pvsRenderSkeletonSceneBlob->GetBufferPointer(),
        m_pvsRenderSkeletonSceneBlob->GetBufferSize(), 
        &m_pVertexLayoutSkeletonMesh ) );

    // Create the constant buffers
    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;

    Desc.ByteWidth = sizeof( VSBufferType );
    V_RETURN( m_pd3dDevice->CreateBuffer( &Desc, NULL, &m_pVSBuffer ) );

    Desc.ByteWidth = sizeof( PSBufferType );
    V_RETURN( m_pd3dDevice->CreateBuffer( &Desc, NULL, &m_pPSBuffer ) );

    return hr;
}

void WEMeshRenderer::Release()
{
    SAFE_RELEASE(m_pVSBuffer);
    SAFE_RELEASE(m_pPSBuffer);

    SAFE_RELEASE(m_pVertexLayoutMesh);
    SAFE_RELEASE(m_pVertexLayoutSkeletonMesh);

    SAFE_RELEASE(m_pvsRenderVarianceShadow);
    SAFE_RELEASE(m_ppsRenderVarianceShadow);
    SAFE_RELEASE(m_pvsRenderVarianceShadowBlob);
    SAFE_RELEASE(m_ppsRenderVarianceShadowBlob);

    SAFE_RELEASE(m_pvsRenderScene);
    SAFE_RELEASE(m_pvsRenderSceneBlob);

    SAFE_RELEASE(m_pvsRenderSkeletonScene);
    SAFE_RELEASE(m_pvsRenderSkeletonSceneBlob);
    SAFE_RELEASE(m_pvsRenderSkeletonVarianceShadow);
    SAFE_RELEASE(m_pvsRenderSkeletonVarianceShadowBlob);

    for( INT index=0; index < MAX_CASCADES; ++index ) {    
        for (int iX = 0; iX < 2; ++iX ) {
            for (int iX2 = 0; iX2 < 2; ++iX2 ) {
                SAFE_RELEASE(m_ppsRenderSceneAllShaders[index][iX][iX2]);
                SAFE_RELEASE(m_ppsRenderSceneAllShadersBlob[index][iX][iX2]);
            }
        }
    }
}

void WEMeshRenderer::DrawCascade(
    WEMesh* pMesh, 
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

void WEMeshRenderer::DrawScene(
    WEMesh* pMesh, 
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
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V( m_pImmediateContext->Map( m_pVSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    VSBufferType* pVSBuffer = ( VSBufferType* )MappedResource.pData;
    pVSBuffer->mWorldViewProj = XMMatrixTranspose( mWorld * mView * mProjection );
    pVSBuffer->mWorldView = XMMatrixTranspose( mWorld * mView );
    pVSBuffer->mWorld = XMMatrixTranspose( mWorld );
    pVSBuffer->mShadow = XMMatrixTranspose( mShadowView );
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
        pPSBuffer->fAlpha = pMaterial->m_fAlpha;
        pPSBuffer->fShininess = pMaterial->m_fShininess;
        pPSBuffer->fStar = pMaterial->m_fStar;
        pPSBuffer->vAmbient = pMaterial->m_vAmbient;
        pPSBuffer->vDiffuse = pMaterial->m_vDiffuse;
        pPSBuffer->vEmissive = pMaterial->m_vEmissive;
        pPSBuffer->vSpecular = pMaterial->m_vSpecular;
        pPSBuffer->iMaterialFlag = pMaterial->GetMaterialFlag();
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
        m_pImmediateContext->PSSetShader( m_ppsRenderSceneAllShaders[uCascadeLevel-1][0][0], NULL, 0 );

        // Draw subset.
        m_pImmediateContext->DrawIndexed( pItem->IndexCount, pItem->IndexStart, pItem->VertexStart );
    }
}


void WEMeshRenderer::DrawCascade(
    WESkeletonMesh* pMesh, 
    CXMMATRIX mWorld, 
    CXMMATRIX mView, 
    CXMMATRIX mProjection)
{
    HRESULT hr;

    // Set input layout.
    m_pImmediateContext->IASetInputLayout(m_pVertexLayoutSkeletonMesh);

    // Set shaders.
    m_pImmediateContext->VSSetShader( m_pvsRenderSkeletonVarianceShadow, NULL, 0 );
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

void WEMeshRenderer::DrawScene(
    WESkeletonMesh* pMesh,
    CXMMATRIX mWorld, 
    CXMMATRIX mView, 
    CXMMATRIX mProjection, 
    CXMMATRIX mShadowView, 
    UINT uCascadeLevel)
{
    HRESULT hr;
    // Set input layout.
    m_pImmediateContext->IASetInputLayout(m_pVertexLayoutSkeletonMesh);

    // Set vertex shader.
    m_pImmediateContext->VSSetShader(m_pvsRenderSkeletonScene, NULL, 0);

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
        pPSBuffer->fAlpha = pMaterial->m_fAlpha;
        pPSBuffer->fShininess = pMaterial->m_fShininess;
        pPSBuffer->fStar = pMaterial->m_fStar;
        pPSBuffer->vAmbient = pMaterial->m_vAmbient;
        pPSBuffer->vDiffuse = pMaterial->m_vDiffuse;
        pPSBuffer->vEmissive = pMaterial->m_vEmissive;
        pPSBuffer->vSpecular = pMaterial->m_vSpecular;
        pPSBuffer->iMaterialFlag = pMaterial->GetMaterialFlag();
        m_pImmediateContext->Unmap( m_pPSBuffer, 0 );
        //CopyMemory(&pPSBuffer->vAmbient, &pMaterial->m_vAmbient, sizeof(XMFLOAT3));
        //CopyMemory(&pPSBuffer->vDiffuse, &pMaterial->m_vDiffuse, sizeof(XMFLOAT3));
        //CopyMemory(&pPSBuffer->vEmissive, &pMaterial->m_vEmissive, sizeof(XMFLOAT3));
        //CopyMemory(&pPSBuffer->vSpecular, &pMaterial->m_vSpecular, sizeof(XMFLOAT3));
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
        m_pImmediateContext->PSSetShader( m_ppsRenderSceneAllShaders[uCascadeLevel-1][0][0], NULL, 0 );

        // Draw subset.
        m_pImmediateContext->DrawIndexed( pItem->IndexCount, pItem->IndexStart, pItem->VertexStart );
    }
}