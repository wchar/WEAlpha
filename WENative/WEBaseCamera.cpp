#include "WEBaseCamera.h"

namespace WE
{
    static WEBaseCamera* g_pCamera = NULL;
    WEBaseCamera* Camera()
    {
#ifdef DEBUG
        if (g_pCamera == NULL)
        {
            MessageBox(NULL,L"Camera is null",L"error",0x00020000L);
            exit(0);
        }
#endif
        return g_pCamera;
    }

    void SetCamera(WEBaseCamera* pCamera)
    {
        g_pCamera = pCamera;
    }

    WEModelViewerCamera* Light()
    {
        static WEModelViewerCamera g_LightCamera;
        return &g_LightCamera;
    }
}

//--------------------------------------------------------------------------------------
// WEBaseCamera
//-------------------------------------------------------------------------------------
WEBaseCamera::WEBaseCamera(void)
{    
    m_vEyePt = XMFLOAT3(0.0f, 0.0f, 10.0f);
    m_vLookatPt = XMFLOAT3(0.0f, 0.0f, 0.0f);    

    // Setup the view matrix
    SetViewParams( &m_vEyePt, &m_vLookatPt );

    // Setup the projection matrix
    SetProjParams( 100, 100, 1.0f, 1.0f, 1000.0f );
}

WEBaseCamera::~WEBaseCamera(void)
{
}


void WEBaseCamera::SetProjParams(UINT width, UINT height, FLOAT fNearPlane, FLOAT fFarPlane, FLOAT fFOV)
{
    // Set attributes for the projection matrix
    m_uWidth = width;
    m_uHeight = height;

    m_fFOV = fFOV;
    m_fAspect = m_uWidth / (FLOAT)m_uHeight;
    m_fNearPlane = fNearPlane;
    m_fFarPlane = fFarPlane;
    
    XMMATRIX mProjXM = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_fAspect, 0.01f, 100.0f);
    XMStoreFloat4x4(&m_mProj, mProjXM);
}

void WEBaseCamera::SetViewParams(XMFLOAT3* pvEyePt, XMFLOAT3* pvLookatPt)
{
    if( NULL == pvEyePt || NULL == pvLookatPt )
        return;

    m_vEyePt = *pvEyePt;
    m_vLookatPt = *pvLookatPt;   

    // Calculate the view matrix.   
    XMVECTOR vEye = XMLoadFloat3(pvEyePt);
    XMVECTOR vAt = XMLoadFloat3(pvLookatPt);
    XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
    XMMATRIX mView = XMMatrixLookAtLH(vEye, vAt, vUp);
    XMMATRIX mViewInv = XMMatrixInverse(&vUp, mView);

    // Store the matrices.
    XMStoreFloat4x4(&m_mView, mView);
    XMStoreFloat4x4(&m_mViewInv, mViewInv);
}


//--------------------------------------------------------------------------------------
// WEModelViewerCamera
//-------------------------------------------------------------------------------------
WEModelViewerCamera::WEModelViewerCamera()
{
    XMStoreFloat4(&m_qNow, XMQuaternionIdentity());
    XMStoreFloat4(&m_qDown, XMQuaternionIdentity());
    XMStoreFloat4x4(&m_mRotation, XMMatrixIdentity());

    m_bDrag = FALSE;

    m_fRadius = 1.215f;

    m_vDownPt = XMFLOAT3(0.0f, 0.0f, 1.0f);
    m_vCurrentPt = XMFLOAT3(0.0f, 0.0f, 1.0f);
    m_fEyeRadius = 6.0;
    OnBegin(0 ,0);
    OnMove(1, 0);
    OnEnd();
}

XMFLOAT3 WEModelViewerCamera::ScreenToVector(float fScreenPtX, float fScreenPtY)
{
    // Scale to screen
    FLOAT x = - (fScreenPtX - m_uWidth / 2) / (m_fRadius * m_uWidth / 2);
    FLOAT y = - (fScreenPtY - m_uHeight / 2) / (m_fRadius * m_uHeight / 2);

    FLOAT z = 0.0f;
    FLOAT mag = x * x + y * y;

    if (mag > 1.0f)
    {
        FLOAT scale = 1.0f / sqrtf(mag);
        x *= scale;
        y *= scale;
    }
    else
        z = sqrtf(1.0f - mag);

    // Return vector
    return XMFLOAT3(x, y, z);
}

XMVECTOR WEModelViewerCamera::QuatFromBallPoints(const XMVECTOR vFrom, const XMVECTOR vTo)
{   
    if (XMVector3Equal(vFrom, vTo))
        return  XMQuaternionIdentity();
    float w = XMVectorGetX(XMVector3AngleBetweenNormals(vFrom, vTo));
    XMVECTOR vPart = XMVector3Cross(vFrom, vTo);
    return XMQuaternionRotationAxis(vPart, w * 2.0f);
}

void WEModelViewerCamera::OnBegin(int nX, int nY)
{
    // Only enter the drag state if the click falls
    // inside the click rectangle.
    if(nX >= 0 &&
        nX < (int)(m_uWidth) &&
        nY >= 0 &&
        nY <  (int)(m_uHeight))
    {
        m_bDrag = true;
        m_qDown = m_qNow;
        m_vDownPt = ScreenToVector((float)nX, (float)nY);
    }
}

void WEModelViewerCamera::OnMove(int nX, int nY)
{
    if (m_bDrag)
    {
        m_vCurrentPt = ScreenToVector((float)nX, (float)nY);

        XMVECTOR qDown = XMLoadFloat4(&m_qDown);
        XMVECTOR vDownPt = XMLoadFloat3(&m_vDownPt);
        XMVECTOR vCurrentPt = XMLoadFloat3(&m_vCurrentPt);
        qDown = XMQuaternionMultiply(qDown, QuatFromBallPoints(vDownPt, vCurrentPt));
        XMStoreFloat4(&m_qNow, qDown);
        // Simply update
        Update();
    }
}

void WEModelViewerCamera::OnEnd()
{
    m_bDrag = false;
}

void WEModelViewerCamera::Update()
{
    XMVECTOR vDeterminant;
    XMMATRIX mCameraRot = GetRotationMatrix();

    mCameraRot = XMMatrixInverse(&vDeterminant, mCameraRot);

    XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, m_fEyeRadius, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1000.0f, 0.0f, 0.0f);
    Eye = XMVector3Transform(Eye, mCameraRot);
    Up = XMVector3Transform(Up, mCameraRot);

    XMMATRIX mView = XMMatrixLookAtLH(Eye, At, Up);
    XMStoreFloat4x4(&m_mView, mView);

    // Store EyePos
    XMStoreFloat3(&m_vEyePt, Eye);

    // mtxViewInv
    XMMATRIX mViewInv = XMMatrixInverse(&vDeterminant, mView);
    XMStoreFloat4x4(&m_mViewInv, mViewInv); 
}

void WEModelViewerCamera::Closer()
{
    m_fEyeRadius *= 1.1f;
    Update();
}

void WEModelViewerCamera::Further()
{
    m_fEyeRadius *= 0.90909f;
    Update();
}