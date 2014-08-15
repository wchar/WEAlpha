#pragma once

#include "WED3D.h"
#include "WEBaseCamera.h"

#include "WEMesh.h"
#include "WESkeletonMesh.h"

class DLL_EXPORT WERenderCore
{
public:
    WERenderCore();
    ~WERenderCore();

    // The hwnd is the window handle will be render for
    HRESULT Create(HWND hwnd);

    //Use this function to change the window/
    HRESULT SetWindow(HWND hwnd);

    // This function should be called when the window was change.
    HRESULT OnResizeWindow();

    // OnFrameRendering
    bool BeginFrame();
    void EndFrame();

    void PushMesh(WEMesh* pMesh)
    {
        m_DrawList_Mesh.push_back(pMesh);
    }

    void PushMesh(WESkeletonMesh* pMesh)
    {
        m_DrawList_SkeletonMesh.push_back(pMesh);
    }
    // SetProperties
    //void EnableShadow(bool bEnable);
    //void CascadeConfig( CascadeConfig& cc );
    //void EnableFlog(bool bEnable);
    //void EnableGlow(bool bEnable);
    //void EnableHDR(bool bEnable);
    //void EnableDOF(bool bEnable);

protected:
    // Draw
    //void DrawMeshCascade( WEMesh* pMesh );
    //void DrawSkeletonMeshCascade( WESkeletonMesh );
    void DrawMesh(WEMesh* pMesh);
    void DrawSkeletonMesh(WESkeletonMesh* pSkeletonMesh);
    //void DrawSkeletonMesh( WESkeletonMesh* pMesh );
    //void DrawParticle( WEParticleSystem* pParticleSystem );
    //void DrawBeam( WEBeam* pBeam );
    //void DrawAnimTrail( WEAnimTrail* pAnimTrail );
    // DrawLists
    void ClearDrawLists();
    vector <WEMesh*> m_DrawList_Mesh;
    vector <WESkeletonMesh*> m_DrawList_SkeletonMesh;
    //vector<WESkeletonMesh*> m_DrawList_SkeletonMesh;
    //vector<WEParticleSystem*> m_DrawList_ParticleSystem;
    //vector<WEBeam*> m_DrawList_Beam;
    //vector<WEAnimTrail*> m_DrawList_AnimTrail;
protected:
    
    // Stage and Process
    enum FrameStage
    {
        eFrameStart = 0,
        eFrameEnd,
    };
    UINT m_uFrameStage;	
};

