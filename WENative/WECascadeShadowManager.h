#pragma once

#include "WED3D.h"
#include "WEMeshRenderer.h"
#include "WECascadeShadowMisc.h"

static const int TARGET_COUNT = 4;

struct CascadeConfig
{
    // Cascade config info
    INT         m_nCascadeLevels;
    INT         m_iBufferSize;

    INT         m_iCascadePartitionsMax;
    FLOAT       m_fCascadePartitionsFrustum[MAX_CASCADES]; // Values are  between near and far
    INT         m_iCascadePartitionsZeroToOne[MAX_CASCADES]; // Values are 0 to 100 and represent a percent of the frstum
    INT         m_iShadowBlurSize;
    INT         m_iBlurBetweenCascades;
    FLOAT       m_fBlurBetweenCascadesAmount;

    BOOL        m_bMoveLightTexelSize;
};

class DLL_EXPORT WECascadeManager
{
public:
    WECascadeManager();
    ~WECascadeManager();

    HRESULT     Create(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
    void        Release();

    void        Render(vector<WEMesh*>* pMeshs, vector<WESkeletonMesh*>* pSkeletonMeshs);

    void        GetRenderTargetSRV();
    void        GetColorSRV();
    void        GetNormalSRV();
    void        GetEmissiveSRV();
    void        GetShadowSRV();
    void        GetSpecularSRV();

    CascadeConfig m_CascadeConfig;

    // This function should be called after m_nCascadeLevels or m_iBufferSize change
    HRESULT     ReleaseAndCreateCascadeBuffers();

    enum
    {
        eColor = 0,
        eNormal,
        eEmissive,
        eDepth,
    };

    ID3D11ShaderResourceView* GetNormalRV()
    {
        return m_pShaderResourceViewArray[eNormal];
    }
    ID3D11ShaderResourceView* GetColorRV()
    {
        return m_pShaderResourceViewArray[eColor];
    }
    ID3D11ShaderResourceView* GetEmissiveRV()
    {
        return m_pShaderResourceViewArray[eEmissive];
    }

    ID3D11ShaderResourceView* GetDepthRV()
    {
        return m_pShaderResourceViewArray[eDepth];
    }

    HRESULT CreateRenderTargetBuffers();
    void ReleaseRenderTargetBuffers();

protected:
    WEMeshRenderer*                 m_pMeshRenderer;
        
    // D3D variables
    ID3D11Device*                   m_pd3dDevice;
    ID3D11DeviceContext*            m_pImmediateContext;

    void ComputeMatrices();
    XMFLOAT4X4                      m_mShadowProj[MAX_CASCADES]; 
    XMFLOAT4X4                      m_mShadowView;

    // This function will be called before render mesh or skeleton each frame.
    HRESULT                         UpdataCascadeConstantBuffer();    
    ID3D11Buffer*                   m_pCascadeConstantBuffer;


    HRESULT                         CreateShaders();
    ID3D11VertexShader*             m_pvsQuadBlur;
    ID3DBlob*                       m_pvsQuadBlurBlob;
    ID3D11PixelShader*              m_ppsQuadBlurX[MAXIMUM_BLUR_LEVELS];
    ID3DBlob*                       m_ppsQuadBlurXBlob[MAXIMUM_BLUR_LEVELS];
    ID3D11PixelShader*              m_ppsQuadBlurY[MAXIMUM_BLUR_LEVELS];
    ID3DBlob*                       m_ppsQuadBlurYBlob[MAXIMUM_BLUR_LEVELS];
 
    ID3D11RasterizerState*          m_prsScene;
    ID3D11RasterizerState*          m_prsShadow;

    ID3D11SamplerState*             m_pSamLinear;
    ID3D11SamplerState*             m_pSamShadowPoint;
    ID3D11SamplerState*             m_pSamShadowLinear;
    ID3D11SamplerState*             m_pSamShadowAnisotropic16;
    ID3D11SamplerState*             m_pSamShadowAnisotropic8;
    ID3D11SamplerState*             m_pSamShadowAnisotropic4;
    ID3D11SamplerState*             m_pSamShadowAnisotropic2;

    ID3D11Texture2D*                m_pCascadedShadowMapVarianceTextureArray;
    ID3D11RenderTargetView*         m_pCascadedShadowMapVarianceRTVArrayAll[MAX_CASCADES];
    ID3D11ShaderResourceView*       m_pCascadedShadowMapVarianceSRVArrayAll[MAX_CASCADES];
    ID3D11ShaderResourceView*       m_pCascadedShadowMapVarianceSRVArraySingle;

    ID3D11Texture2D*                m_pTemporaryShadowDepthBufferTexture;
    ID3D11DepthStencilView*         m_pTemporaryShadowDepthBufferDSV;
                                    
    ID3D11Texture2D*                m_pCascadedShadowMapTempBlurTexture;
    ID3D11RenderTargetView*         m_pCascadedShadowMapTempBlurRTV;
    ID3D11ShaderResourceView*       m_pCascadedShadowMapTempBlurSRV;

    D3D11_VIEWPORT                  m_RenderVP[MAX_CASCADES];
    D3D11_VIEWPORT                  m_RenderOneTileVP;

    // Render target buffers
    ID3D11Texture2D*                m_pRenderTargetTextureArray[TARGET_COUNT];
    ID3D11RenderTargetView*         m_pRenderTargetViewArray[TARGET_COUNT];
    ID3D11ShaderResourceView*       m_pShaderResourceViewArray[TARGET_COUNT];
    ID3D11Texture2D*                m_pDepthBuffer;
    ID3D11DepthStencilView*         m_pDepthStencilView;
    ID3D11ShaderResourceView*       m_pDepthShaderResourceView;
    
    D3D11_VIEWPORT                  m_pViewport;
    ID3D11Buffer*                   m_pScreenSpaceVertexBuffer;
};