#pragma once

#include "WEFbxCommon.h"
#include "WED3D.h"
#include "WEMeshMisc.h"



class WEFbxLoader
{
public:
    WEFbxLoader(WCHAR* strFileName);
    ~WEFbxLoader();

    struct BYTE4
    {
        BYTE _data[4];

        operator BYTE*()
        {
            return _data;
        }
    };

    struct Vertex_Bone_Weight
    {
        int bone[4];
        double weight[4]; 

        Vertex_Bone_Weight()
        {
            ZeroMemory(this, sizeof(Vertex_Bone_Weight));
        }
    };

private:
    void Load(WCHAR* strFileName);

    UINT ParseJoints(FbxNode* pNode, int parent, int sibling, bool bChild, bool bSibling);
    void LoadJoint(FbxNode* pNode); 
    void ParseAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode);
    void ParseAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode);
    void ParseAnimation(FbxScene* pScene);
    bool GetTextureFile(FbxProperty* lProperty, WCHAR* wstrFileName);
    void FillMaterialStructure(MATERIAL* pMaterial, FbxSurfaceMaterial *lFbxMaterial);
    UINT ParseMaterial(FbxGeometry* pGeometry);
    void ParseMesh(FbxMesh* pMesh);
    void LoadMesh(FbxNode* pNode);
    UINT FindJointIndex(const char* strJointName);
    void UpdateLinkMatrix(int jointIdx, FbxCluster* pCluster);
    void StoreMatrix(XMFLOAT4X4* pmInOut, FbxMatrix FbxMtx);
    vector<Vertex_Bone_Weight> * LoadBoneWeight(FbxMesh* pMesh);
protected:
    UINT GetVertex(VERTEX_STATIC* pVertex, UINT idx);
    UINT GetVertex(VERTEX_SKELETON* pVertex, UINT idx);
    vector <UINT> m_arrvIdxControlPoint;
    vector <XMFLOAT3> m_arrvPosition;
    vector <XMFLOAT3> m_arrvNormal;
    vector <XMFLOAT2> m_arrvTexcoord;
    vector <BYTE4> m_arrvWeight;
    vector <BYTE4> m_arrvBone;

    vector <UINT> m_arrvTriangleMaterialIndex;
    vector <MATERIAL> m_arrvMaterial;

    vector <JOINT> m_arrvJoint;
    vector <XMFLOAT4X4> m_arrvGlobalMatrix;
  
    UINT m_uNumFrame;
    UINT m_uFramePreSecond;

    bool m_bResult;

    friend class WEFbxImporter;
};

