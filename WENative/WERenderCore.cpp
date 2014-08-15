#include "WED3DWindow.h"

#include "WERenderCore.h"
#include "WECascadeShadowManager.h"
#include "WEDeferredLighting.h"
#include "WEHDAO.h"
#include "WEFullScreenQuad.h"
#include "WEHDR.h"
#include "WEFinalPass.h"

namespace
{
    WECascadeManager* GetCascadeManager()
    {
        static WECascadeManager g_WECascadeManager;
        return &g_WECascadeManager;
    }

    WEDeferredLighting* GetDeferredLighting()
    {
        static WEDeferredLighting g_DefferredLighting;
        return &g_DefferredLighting;
    }

    WEHDAO* GetHDAO()
    {
        static WEHDAO g_HDAO;
        return &g_HDAO;
    }

    UINT& RenderCoreRefCount() 
    {
        static UINT uRefCount = 0;
        return uRefCount;
    }

    WEHDR* GetHDR()
    {
        static WEHDR g_HDR;
        return &g_HDR;
    }

    WEFullScreenQuad* GetFullScreenQuad()
    {
        static WEFullScreenQuad g_FillScreenQuad;
        return &g_FillScreenQuad;
    }

    WEFinalPass* GetFinalPass()
    {
        static WEFinalPass g_FinalPass;
        return &g_FinalPass;
    }
}

WERenderCore::WERenderCore()
{
    RenderCoreRefCount()++;
    if (RenderCoreRefCount() > 1)
    {
        MessageBox(NULL, L"Only one WERenderCore instance can be created", L"error", 0x00020000L);
        exit(0);
    }

    m_uFrameStage = eFrameEnd;
}

WERenderCore::~WERenderCore()
{
    RenderCoreRefCount()--;
}

HRESULT WERenderCore::Create(HWND hwnd)
{
    HRESULT hr;

    if (RenderCoreRefCount() > 1)
    {
        MessageBox(NULL, L"Only one WERenderCore instance can be created", L"error", 0x00020000L);
        return S_FALSE;
    }

    // Create D3D.
    WE::D3D().Create(hwnd);

    // Get d3dDevice
    ID3D11Device* pd3dDevice = WE::D3DDevice();
    ID3D11DeviceContext* pImmediateContext = WE::ImmediateContext();

    // Create mesh cascade shadow manager.
    V_RETURN(GetCascadeManager()->Create(pd3dDevice, pImmediateContext));

    // Create deferred lighting
    V_RETURN(GetDeferredLighting()->Create(pd3dDevice, pImmediateContext));

    // Create HDAO
    V_RETURN(GetHDAO()->Create(pd3dDevice, pImmediateContext));

    // Create HRD
    V_RETURN(GetHDR()->Create(pd3dDevice, pImmediateContext));

    // Create FullScreenQuad
    V_RETURN(GetFullScreenQuad()->Create(pd3dDevice, pImmediateContext));

    // Create FinalPass
    V_RETURN(GetFinalPass()->Create(pd3dDevice, pImmediateContext));

    WEBaseCamera* pLightCamera = WE::Light();

    return S_OK;
}

HRESULT WERenderCore::SetWindow(HWND hwnd)
{
    HRESULT hr;

    if (m_uFrameStage == eFrameEnd)
    {
        GetCascadeManager()->ReleaseRenderTargetBuffers();

        WE::D3D().SetWindow(hwnd);

        V_RETURN(GetCascadeManager()->CreateRenderTargetBuffers());
    }

    return S_OK;
}

HRESULT WERenderCore::OnResizeWindow()
{
    HRESULT hr;

    if (m_uFrameStage == eFrameEnd)
    {
        GetCascadeManager()->ReleaseRenderTargetBuffers();

        WE::D3D().OnResizeWindow();

        V_RETURN(GetCascadeManager()->CreateRenderTargetBuffers());
    }
    return S_OK;
}

void WERenderCore::ClearDrawLists()
{
    m_DrawList_Mesh.clear();
    m_DrawList_SkeletonMesh.clear();
}

bool WERenderCore::BeginFrame()
{
    if (m_uFrameStage != eFrameEnd)
        return false;
    // SetFrameStage    

    WE::D3D().Clear();

    m_uFrameStage = eFrameStart;

    return true;
}

void WERenderCore::EndFrame()
{
    // Draw mesh and skeleton.
    GetCascadeManager()->Render(&m_DrawList_Mesh, &m_DrawList_SkeletonMesh);
    
    ID3D11ShaderResourceView* pColorRV = GetCascadeManager()->GetColorRV();
    ID3D11ShaderResourceView* pNormalRV = GetCascadeManager()->GetNormalRV();
    ID3D11ShaderResourceView* pDepthRV = GetCascadeManager()->GetDepthRV();
    
    // Deferred lighting.
    if (GetAsyncKeyState( 'A' ))
    {    
        GetDeferredLighting()->Process(GetFullScreenQuad(), pColorRV, pNormalRV, XMFLOAT3(0,0,1));
        pColorRV = GetDeferredLighting()->GetLightRV();
    }
    else if (GetAsyncKeyState( 'B' ))
    {
        GetHDR()->Process(GetFullScreenQuad(), pColorRV);    
        pColorRV = GetHDR()->GetLumRV();
    }
    else if (GetAsyncKeyState( 'C' ))
    {
        GetHDAO()->Process(GetFullScreenQuad(), pNormalRV, pDepthRV);    
        pColorRV = GetHDAO()->GetOcclusionRV();
    }
         
    ID3D11RenderTargetView* pRTV = WE::D3D().GetRenderTargetView();
    GetFinalPass()->TestShaderResourceView(GetFullScreenQuad(), pRTV, pColorRV);


    // Clearing and Ending    
    WE::D3D().Present();
    ClearDrawLists();
    m_uFrameStage = eFrameEnd;
}

void WERenderCore::DrawMesh(WEMesh* pMesh)
{
    m_DrawList_Mesh.push_back(pMesh);
}

void WERenderCore::DrawSkeletonMesh(WESkeletonMesh* pSkeletonMesh)
{
    m_DrawList_SkeletonMesh.push_back(pSkeletonMesh);
}