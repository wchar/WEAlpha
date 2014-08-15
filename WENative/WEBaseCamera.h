#pragma once

#include "WED3D.h"

class WEBaseCamera;
class WEModelViewerCamera;
class WEFirstPersonCamera;

namespace WE
{
    DLL_EXPORT WEBaseCamera* Camera();
    DLL_EXPORT void SetCamera(WEBaseCamera* pCamera);
    DLL_EXPORT WEModelViewerCamera* Light();
}



class DLL_EXPORT WEBaseCamera
{
public:
    WEBaseCamera();
    virtual ~WEBaseCamera();

    // Functions to get state.
    const XMMATRIX GetViewMtx() const			
    {
        return XMLoadFloat4x4(&m_mView);
    }
    const XMMATRIX GetProjMtx() const				
    {			
        return XMLoadFloat4x4(&m_mProj);				
    }
    const XMMATRIX GetViewProjMtx() const		
    {			
        return GetViewMtx()*GetProjMtx();				
    }
    const XMMATRIX GetViewInvMtx() const				
    {			
        return XMLoadFloat4x4(&m_mViewInv);		
    }
    const XMFLOAT3& GetEyePosition() const
    {			
        return m_vEyePt;										
    }
    const XMFLOAT3& GetLookatPosition()	const		
    {		
        return m_vLookatPt;								
    }
    float GetNearClip() const
    {
        return m_fNearPlane;
    }
    float GetFarClip() const
    {
        return m_fFarPlane;
    }


    // Functions to change camera matrices.
    void SetViewParams(XMFLOAT3* pvEyePt, XMFLOAT3* pvLookatPt);
    void SetProjParams(UINT width, UINT height, FLOAT fNearPlane, FLOAT fFarPlane, FLOAT fFOV = XM_PIDIV4);

protected:
    XMFLOAT4X4      m_mView;
    XMFLOAT4X4      m_mProj;
    XMFLOAT4X4      m_mViewInv;    

    XMFLOAT3        m_vEyePt;
    XMFLOAT3        m_vLookatPt;

    UINT            m_uWidth;
    UINT            m_uHeight;

    FLOAT           m_fNearPlane;
    FLOAT           m_fFarPlane;
    FLOAT           m_fFOV;
    FLOAT           m_fAspect;
};




class DLL_EXPORT WEModelViewerCamera : public WEBaseCamera
{
public:
    WEModelViewerCamera();

    // Functions to control camera.
    void Closer();
    void Further();

    void OnBegin(int nX, int nY); 
    void OnMove(int nX, int nY); 
    void OnEnd(); 


    static XMVECTOR	QuatFromBallPoints(const XMVECTOR vFrom, const XMVECTOR vTo);
protected:
    void Update();

    XMFLOAT4X4	    m_mRotation;

    FLOAT			m_fRadius;
    FLOAT			m_fEyeRadius;

    XMFLOAT4		m_qDown;
    XMFLOAT4		m_qNow;
    bool			m_bDrag;

    XMFLOAT3		m_vDownPt;
    XMFLOAT3		m_vCurrentPt;
    XMFLOAT3		ScreenToVector(float fScreenPtX, float fScreenPtY);    
    
    const XMMATRIX GetRotationMatrix()			
    {			
        return XMMatrixRotationQuaternion(XMLoadFloat4(&m_qNow));		
    }
    bool IsBeingDragged() const							
    {			
        return m_bDrag;
    }
    const XMFLOAT4 GetQuatNow() const			
    {			
        return m_qNow;
    }
    void SetQuatNow(const XMFLOAT4* q)		
    {			
        m_qNow = *q;	
    }
};



class WEFirstPersonCamera : public WEBaseCamera
{
public:

protected:

};