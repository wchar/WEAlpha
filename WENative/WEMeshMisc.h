#pragma once

#include "WED3D.h"

#define MAX_JOINT 80
#define MAX_SUBSET 20

struct VERTEX_STATIC
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 texcoord;    
    XMFLOAT3 tangent;
};

struct VERTEX_SKELETON
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 texcoord;
    XMFLOAT3 tangent;
    BYTE weight[4];
    BYTE bone[4];
};

struct MATERIAL
{
    XMFLOAT3 vAmbient;
    XMFLOAT3 vDiffuse;
    XMFLOAT3 vEmissive;
    XMFLOAT3 vSpecular;

    float fOpacity;
    float fShininess;
    float fReflectivity;

    bool bSpecular;// false Lambert and true Phong.

    bool bDiffuseTexture;
    bool bEmissiveTexture;
    bool bNormalMap;
    WCHAR strDiffuseTexture[MAX_PATH];
    WCHAR strEmissiveTexture[MAX_PATH];
    WCHAR strNormalMap[MAX_PATH];

    WCHAR strMaterialName[MAX_PATH];
};

struct JOINT
{
    UINT uParentJoint;
    UINT uChildJoint;
    UINT uSiblingJoint;
    XMFLOAT4X4 Matrix;

    WCHAR strName[MAX_PATH];

    JOINT()
    {
        uParentJoint = -1;
        uChildJoint = -1;
        uSiblingJoint = -1;
    }
};

class WEAnimation
{
public:
    UINT uJointCount;
    JOINT mJoints[MAX_JOINT];

    UINT uFrameCount;
    UINT uFramePreSecond;
    XMFLOAT4X4* mGlobals;

    WEAnimation():mGlobals(NULL){}
    ~WEAnimation(){SAFE_DELETE(mGlobals)}

    void Init(const WEAnimation& another)
    {
        SAFE_DELETE(mGlobals);

        UINT sizeofStruct = sizeof(WEAnimation);
        CopyMemory(this, &another, sizeof(WEAnimation));

        mGlobals = new XMFLOAT4X4[uJointCount * uFrameCount];
        UINT sizeofMatrices = sizeof(XMFLOAT4X4) * uJointCount * uFrameCount;
        CopyMemory(mGlobals, another.mGlobals, sizeofMatrices);
    }

    UINT Size()
    {
        return sizeof(WEAnimation) + sizeof(XMFLOAT4X4) * uJointCount * uFrameCount;
    }

    void CopyFrom(BYTE* pBUf)
    {      
        SAFE_DELETE(mGlobals);

        UINT sizeofStruct = sizeof(WEAnimation);
        CopyMemory(this, pBUf, sizeof(WEAnimation));
        pBUf += sizeofStruct;
 
        mGlobals = new XMFLOAT4X4[uJointCount * uFrameCount];
        UINT sizeofMatrices = sizeof(XMFLOAT4X4) * uJointCount * uFrameCount;
        CopyMemory(mGlobals, pBUf, sizeofMatrices);
    }

    void CopyTo(BYTE* pBUf)
    {
        UINT sizeofStruct = sizeof(WEAnimation);
        UINT sizeofMatrices = sizeof(XMFLOAT4X4) * uJointCount * uFrameCount;
        CopyMemory(pBUf, this, sizeofStruct);
        pBUf += sizeofStruct;
        CopyMemory(pBUf, mGlobals, sizeofMatrices);
    }

    const XMFLOAT4X4& GetGlobalMatrix(UINT iFrame, UINT iJoint)
    {
        return mGlobals[iFrame * uJointCount + iJoint];
    }

    const XMFLOAT4X4& GetGlobalMatrix(UINT idx)
    {
        return mGlobals[idx];
    }
};

struct MESH_SUBSET
{
    UINT IndexCount;
    UINT IndexStart;
    UINT VertexStart;
    UINT MaterialID;
};

struct MESH_INFO
{       
    UINT uVertexCount;
    UINT uIndexCount;
    UINT uSubsetCount;   
    UINT uMaterialCount;
};

struct HEAD_STATIC
{
    UINT uVertexOffset;
    UINT uIndexOffset;
    UINT uMaterialOffset;

    MESH_INFO mMeshInfo;

    MESH_SUBSET mSubsets[MAX_SUBSET];
    UINT uMaterialDataOffsets[MAX_SUBSET];
};

struct HEAD_SKELETON
{
    UINT uVertexOffset;
    UINT uIndexOffset;
    UINT uAnimOffset;
    UINT uMaterialOffset;

    MESH_INFO mMeshInfo;

    MESH_SUBSET mSubsets[MAX_SUBSET];
    UINT uMaterialDataOffsets[MAX_SUBSET];
};