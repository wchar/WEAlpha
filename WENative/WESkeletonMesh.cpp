#include "WESkeletonMesh.h"


WESkeletonMesh::WESkeletonMesh(WESkeletonMeshContent* pContent)
{
    HRESULT hr;

    // MeshInfo
    CopyMemory(&m_MeshInfo, &pContent->m_Head.mMeshInfo, sizeof(MESH_INFO));

    // Create VB
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.ByteWidth = (UINT)( GetVBStride() * m_MeshInfo.uVertexCount );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = pContent->GetVerticesPtr();
    V(WE::D3DDevice()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

    // Create IB
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = (UINT)( sizeof(DWORD) * m_MeshInfo.uIndexCount );
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = pContent->GetIndicesPtr();
    (WE::D3DDevice()->CreateBuffer(&bd, &InitData, &m_pIndexBuffer));

    // Craete Materials
    ReleaseMaterials();
    vector<WEMaterialContent*>* contents = &pContent->m_MaterialContents;
    for (vector<WEMaterialContent*>::iterator it = contents->begin();
        it != contents->end(); it++ )
    {
        m_Materials.push_back( (*it)->CreateMaterial() );
    }

    // Create Subsets
    ReleaseSubsets();
    for (int i = 0; i < m_MeshInfo.uSubsetCount; i++)
    { 
        MESH_SUBSET* pSubset = new MESH_SUBSET();
        CopyMemory(pSubset, &pContent->m_Head.mSubsets[i], sizeof(MESH_SUBSET));
        m_Subsets.push_back(pSubset);
    }

    m_AnimationInfo.Init(pContent->m_AnimationInfo);

    TransPose(0);
}


WESkeletonMesh::~WESkeletonMesh()
{
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
    
    ReleaseMaterials();
    ReleaseSubsets();
}

void WESkeletonMesh::ReleaseMaterials()
{
    for (vector<WEMaterial*>::iterator it = m_Materials.begin();
        it != m_Materials.end(); it++)
    {
        SAFE_DELETE(*it);
    }
    m_Materials.clear();
}

void WESkeletonMesh::ReleaseSubsets()
{
    for (vector<MESH_SUBSET*>::iterator it = m_Subsets.begin();
        it != m_Subsets.end(); it++)
    {
        SAFE_DELETE(*it);
    }
    m_Subsets.clear();
}

void WESkeletonMesh::TransPose(UINT uJoint)
{
    JOINT& joint = m_AnimationInfo.mJoints[uJoint];
    XMMATRIX JointLink = XMLoadFloat4x4(&joint.Matrix);
    m_MatrixOff[uJoint] = JointLink;

    // Transform our siblings
    if( joint.uSiblingJoint != -1 )
        TransPose(joint.uSiblingJoint);

    // Transform our children
    if( joint.uChildJoint != -1 )
        TransPose(joint.uChildJoint);
}

void WESkeletonMesh::TransformMesh(double fTime)
{
    TransformFrame(0, fTime);
    XMVECTOR vDeterminant;
    XMMATRIX mInvBindPose;
    XMMATRIX mFinal;
    for (int i = m_AnimationInfo.uJointCount - 1; i >= 0; i--)
    {
        mInvBindPose = XMMatrixInverse(&vDeterminant, m_MatrixOff[i]);
        mFinal = mInvBindPose * m_Transform[i];
        m_MatrixFinal[i] = mFinal;
    }
}

void WESkeletonMesh::TransformFrame(UINT uJoint, double fTime)
{
    UINT iTick = FindAnimMatrix(fTime);
    //UINT idx = iTick * m_AnimationInfo.uJointCount + uJoint;
    //XMFLOAT4X4* pmLocalTransform = &m_AnimationInfo.mGlobals[idx];
    const XMFLOAT4X4& mGlobal = m_AnimationInfo.GetGlobalMatrix(iTick, uJoint);

    m_Transform[uJoint] = XMLoadFloat4x4(&mGlobal);
        
    JOINT& joint = m_AnimationInfo.mJoints[uJoint];
    
    // Transform our siblings
    if (joint.uSiblingJoint != -1)
        TransformFrame(joint.uSiblingJoint, fTime);

    // Transform our children
    if (joint.uChildJoint != -1)
        TransformFrame(joint.uChildJoint, fTime);
}

UINT WESkeletonMesh::FindAnimMatrix(double fTime)
{
    UINT iTick = (UINT)(fTime * m_AnimationInfo.uFramePreSecond * 4);
    iTick = iTick % (m_AnimationInfo.uFrameCount);
    return iTick;
}