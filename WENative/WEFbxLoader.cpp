#include "WEFbxLoader.h"


WEFbxLoader::WEFbxLoader(WCHAR* strFileName)
{
    m_uFramePreSecond = 30;
    m_uNumFrame = 0;

    m_bResult = false;
    Load(strFileName);
}

WEFbxLoader::~WEFbxLoader()
{

}

void WEFbxLoader::Load(WCHAR* strFileName)
{
    char str[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, strFileName, -1, str, MAX_PATH, NULL, NULL);

    FbxManager* lSdkManager;
    FbxScene* lScene;

    // Prepare the FBX SDK.
    InitializeSdkObjects(lSdkManager, lScene);

    FbxString lFilePath(str);
    if(lFilePath.IsEmpty())
    {
        m_bResult = false;
    }
    else
    {
        m_bResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());
    }
    if(m_bResult)
    {
        FbxNode* lNode = lScene->GetRootNode();

        if(lNode)
        {
            //
            for (int i = 0; i < lNode->GetChildCount(); i++)
            {
                LoadJoint(lNode->GetChild(i));               
            }
            //
            for (int i = 0; i < lNode->GetChildCount(); i++)
            {
                LoadMesh(lNode->GetChild(i));
            }       
        }
        //
        ParseAnimation(lScene);
    }

    // Destroy all objects created by the FBX SDK.
    DestroySdkObjects(lSdkManager, true);
}

UINT WEFbxLoader::ParseJoints(FbxNode* pNode, int parent, int sibling, bool bChild, bool bSibling)
{
    int self = m_arrvJoint.size();

    m_arrvJoint.push_back(JOINT());
    JOINT* pSelf = &m_arrvJoint.back();

    MultiByteToWideChar(CP_ACP, 0, pNode->GetName(), -1, pSelf->strName, MAX_PATH);

    pSelf->uParentJoint = parent;

    if (bChild)
    {
        m_arrvJoint[parent].uChildJoint = self;
    }

    if (bSibling)
    {
        m_arrvJoint[sibling].uSiblingJoint = self;
    }

    for(int i = 0, last = -1, j = 0; i < pNode->GetChildCount(); i++)
    {
        if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
        {
            last = ParseJoints(pNode->GetChild(i), self, last, (j == 0), !(j == 0));
            j++;
        }
    }

    return self;
}

void WEFbxLoader::LoadJoint(FbxNode* pNode)
{
    FbxNodeAttribute::EType lAttributeType;
    int i;

    if(pNode->GetNodeAttribute() != NULL)
    {
        lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

        switch (lAttributeType)
        {  
        case FbxNodeAttribute::eSkeleton:  
            if (pNode->GetSkeleton()->IsSkeletonRoot())
            {
                ParseJoints(pNode,-1, -1, false, false);                 
            }
            break;
        }   
    }

    for(i = 0; i < pNode->GetChildCount(); i++)
    {
        LoadJoint(pNode->GetChild(i));
    }
}

void WEFbxLoader::ParseAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode)
{
    int lModelCount;
    FbxString lOutputString;

    // DisplayChannels(pNode, pAnimLayer, DisplayCurveKeys, DisplayListCurveKeys, true);
    if ( pNode->GetNodeAttribute() != NULL && pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton )
    {
        UINT jointIdx = FindJointIndex(pNode->GetName());
        UINT jointCount = m_arrvJoint.size();

        if ( jointIdx >= 0 )
        {
            FbxTime keyTimer;
            XMFLOAT4X4* pMtxLocation;
            for (int i = 0; i < m_uNumFrame; i++)
            {
                keyTimer.SetFrame(i);
                pMtxLocation = &m_arrvGlobalMatrix[i * jointCount + jointIdx];
                StoreMatrix(pMtxLocation, pNode->EvaluateGlobalTransform(keyTimer));
            }
        }
    }

    for(lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
    {
        ParseAnimation(pAnimLayer, pNode->GetChild(lModelCount));
    }
}

void WEFbxLoader::ParseAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode)
{
    int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();

    for (int l = 0; l < nbAnimLayers; l++)
    {
        FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);

        ParseAnimation(lAnimLayer, pNode);
    }
}

void WEFbxLoader::ParseAnimation(FbxScene* pScene)
{
    FbxGlobalSettings* pGlobalSettings = &pScene->GetGlobalSettings();
    
    FbxTimeSpan lTs;
    pGlobalSettings->GetTimelineDefaultTimeSpan(lTs);
    FbxTime lStart = lTs.GetStart();
    FbxTime lEnd = lTs.GetStop();

    UINT uStart = lStart.GetFrameCount();
    UINT uEnd = lEnd.GetFrameCount();
    m_uNumFrame = uEnd - uStart;

    UINT matrixSize = m_uNumFrame * m_arrvJoint.size();
    m_arrvGlobalMatrix.resize(matrixSize);

    for (int i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
    {
        FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);

        ParseAnimation(lAnimStack, pScene->GetRootNode());
    }
}

UINT GetPosition(FbxMesh* pMesh, XMFLOAT3* vOut, int iPolygon, int iVertex)
{
    UINT lControlPointIndex = pMesh->GetPolygonVertex(iPolygon, iVertex);
    FbxVector4* lControlPoints = pMesh->GetControlPoints(); 
    FbxVector4 pos = lControlPoints[lControlPointIndex];
    vOut->x = pos[0];
    vOut->y = pos[1];
    vOut->z = pos[2];

    return lControlPointIndex;
}

void GetUV(FbxMesh* pMesh, XMFLOAT2* vOut, int iPolygon, int iVertex)
{
    FbxVector2 texcoord;

    int lControlPointIndex = pMesh->GetPolygonVertex(iPolygon, iVertex);
    for (int l = 0; l < pMesh->GetElementUVCount(); ++l)
    {
        FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);

        switch (leUV->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            switch (leUV->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                texcoord = leUV->GetDirectArray().GetAt(lControlPointIndex);
                break;
            case FbxGeometryElement::eIndexToDirect:
                {
                    int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
                    texcoord = leUV->GetDirectArray().GetAt(id);
                }
                break;
            default:
                break; // other reference modes not shown here!
            }
            break;

        case FbxGeometryElement::eByPolygonVertex:
            {
                int lTextureUVIndex = pMesh->GetTextureUVIndex(iPolygon, iVertex);
                switch (leUV->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                case FbxGeometryElement::eIndexToDirect:
                    {
                        texcoord = leUV->GetDirectArray().GetAt(lTextureUVIndex);
                    }
                    break;
                default:
                    break; // other reference modes not shown here!
                }
            }
            break;

        case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
        case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
        case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
            break;
        }
    } 

    vOut->x = texcoord[0];
    vOut->y = texcoord[1];
}

void GetNormal(FbxMesh* pMesh, XMFLOAT3* vOut, int iPolygon, int iVertex)
{
    FbxVector4 normal;

    pMesh->GetPolygonVertexNormal(iPolygon, iVertex, normal);

    vOut->x = normal[0];
    vOut->y = normal[1];
    vOut->z = normal[2];

}

UINT WEFbxLoader::FindJointIndex(const char* strJointName)
{
    WCHAR jn[MAX_PATH];

    MultiByteToWideChar(CP_ACP, 0, strJointName, -1, jn, MAX_PATH);

    int searchTime = 0;
    for (int i = 0; i < m_arrvJoint.size(); i++)
    {
        if (wcscmp(m_arrvJoint[i].strName, jn) == 0)
        {
            return searchTime;
        }
        searchTime++;      
    }

    return -1;
}

void WEFbxLoader::StoreMatrix(XMFLOAT4X4* pmInOut, FbxMatrix FbxMtx)
{    
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            pmInOut->m[i][j] = FbxMtx[i][j];
        }
    }
}

void WEFbxLoader::UpdateLinkMatrix(int jointIdx, FbxCluster* pCluster)
{
    FbxAMatrix ml;
    pCluster->GetTransformLinkMatrix(ml);

    StoreMatrix(&m_arrvJoint[jointIdx].Matrix, ml);
}

vector<WEFbxLoader::Vertex_Bone_Weight>* WEFbxLoader::LoadBoneWeight(FbxMesh* pMesh)
{
    // Load bone data.
    FbxCluster* lCluster;

    vector <Vertex_Bone_Weight>* arrv = new vector <Vertex_Bone_Weight>(pMesh->GetControlPointsCount());
    
    UINT uSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

    for (UINT i = 0; i != uSkinCount; ++i)
    {
        UINT uClusterCount = ((FbxSkin *) pMesh->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
        for (UINT j = 0; j != uClusterCount; ++j)
        {
            lCluster=((FbxSkin *) pMesh->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);

            int jointIdx = FindJointIndex(lCluster->GetLink()->GetName());


            if (jointIdx < 0)
            {
                continue;
            }

            UpdateLinkMatrix(jointIdx, lCluster);

            int k, m, n, lIndexCount = lCluster->GetControlPointIndicesCount();
            int* lIndices = lCluster->GetControlPointIndices();
            double* lWeights = lCluster->GetControlPointWeights();

            for (k = 0; k < lIndexCount; k++)
            {
                int index = lIndices[k];                    
                double weight = lWeights[k];   

                int* pBones = arrv->at(index).bone;
                double* pWeights = arrv->at(index).weight;

                for (m = 0; m < 4; m++)
                {
                    if (weight > pWeights[m])
                    {
                        for(n = 3; n > m; n--)
                        {
                            pBones[n] = pBones[n-1];
                            pWeights[n] = pWeights[n-1];
                        }
                        pBones[m] = jointIdx;
                        pWeights[m] = weight;

                        break;
                    }
                }
            }
        }
    }

    return arrv;
}

bool WEFbxLoader::GetTextureFile(FbxProperty* lProperty, WCHAR* wstrFileName)
{
    if (!lProperty->IsValid())
        return false;

    int lTextureCount = lProperty->GetSrcObjectCount<FbxTexture>();

    for (int j = 0; j < lTextureCount; ++j)
    {
        //Here we have to check if it's layeredtextures, or just textures:
        FbxLayeredTexture *lLayeredTexture = lProperty->GetSrcObject<FbxLayeredTexture>(j);
        if (lLayeredTexture)
        {
            FbxLayeredTexture *lLayeredTexture = lProperty->GetSrcObject<FbxLayeredTexture>(j);
            int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
            for(int k =0; k<lNbTextures; ++k)
            {
                FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
                if(lTexture)
                {
                    FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
                    if (lFileTexture)
                    {
                        const char* cstrFileName = lFileTexture->GetFileName();

                        MultiByteToWideChar(CP_ACP, 0, cstrFileName, -1, wstrFileName, MAX_PATH);

                        return true;
                    } 
                }
            }
        }
        else
        {
            //no layered texture simply get on the property
            FbxTexture* lTexture = lProperty->GetSrcObject<FbxTexture>(j);
            if(lTexture)
            {    
                FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
                if (lFileTexture)
                {
                    const char* cstrFileName = lFileTexture->GetFileName();

                    MultiByteToWideChar(CP_ACP, 0, cstrFileName, -1, wstrFileName, MAX_PATH);

                    return true;
                } 
            }

        }
    }

    return false;
}

void WEFbxLoader::FillMaterialStructure(MATERIAL* pMaterial, FbxSurfaceMaterial *lFbxMaterial)
{
    FbxPropertyT<FbxDouble3> lKFbxDouble3;
    FbxPropertyT<FbxDouble> lKFbxDouble1;
    FbxColor theColor;

    ZeroMemory(pMaterial,sizeof(MATERIAL));

    const char* cstrmtlName = lFbxMaterial->GetName();
    MultiByteToWideChar(CP_ACP, 0, cstrmtlName, -1, pMaterial->strMaterialName, MAX_PATH);

    if (lFbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
    {
        // We found a Phong material.  Display its properties.

        // Display the Ambient Color
        lKFbxDouble3 =((FbxSurfacePhong *) lFbxMaterial)->Ambient;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->vAmbient.x = (float)theColor.mRed;
        pMaterial->vAmbient.y = (float)theColor.mGreen;
        pMaterial->vAmbient.z = (float)theColor.mBlue;

        // Display the Diffuse Color
        lKFbxDouble3 =((FbxSurfacePhong *) lFbxMaterial)->Diffuse;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->vDiffuse.x = (float)theColor.mRed;
        pMaterial->vDiffuse.y = (float)theColor.mGreen;
        pMaterial->vDiffuse.z = (float)theColor.mBlue;

        // Display the Specular Color (unique to Phong materials)
        lKFbxDouble3 =((FbxSurfacePhong *) lFbxMaterial)->Specular;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->vSpecular.x = (float)theColor.mRed;
        pMaterial->vSpecular.y = (float)theColor.mGreen;
        pMaterial->vSpecular.z = (float)theColor.mBlue;

        // Display the Emissive Color
        lKFbxDouble3 =((FbxSurfacePhong *) lFbxMaterial)->Emissive;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->vEmissive.x = (float)theColor.mRed;
        pMaterial->vEmissive.y = (float)theColor.mGreen;
        pMaterial->vEmissive.z = (float)theColor.mBlue;

        //Opacity is Transparency factor now
        lKFbxDouble1 =((FbxSurfacePhong *) lFbxMaterial)->TransparencyFactor;

        pMaterial->fOpacity = 1.0f-(float)lKFbxDouble1.Get();

        // Display the Shininess
        lKFbxDouble1 =((FbxSurfacePhong *) lFbxMaterial)->Shininess;

        pMaterial->fShininess = (float)lKFbxDouble1.Get();

        // Display the Reflectivity
        lKFbxDouble1 =((FbxSurfacePhong *) lFbxMaterial)->ReflectionFactor;

        pMaterial->fReflectivity = (float)lKFbxDouble1.Get();

        pMaterial->bSpecular = true;

    }
    else if(lFbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) )
    {
        // We found a Lambert material. Display its properties.
        // Display the Ambient Color
        lKFbxDouble3=((FbxSurfaceLambert *)lFbxMaterial)->Ambient;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->vAmbient.x = (float)theColor.mRed;
        pMaterial->vAmbient.y = (float)theColor.mGreen;
        pMaterial->vAmbient.z = (float)theColor.mBlue;

        // Display the Diffuse Color
        lKFbxDouble3 =((FbxSurfaceLambert *)lFbxMaterial)->Diffuse;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->vDiffuse.x = (float)theColor.mRed;
        pMaterial->vDiffuse.y = (float)theColor.mGreen;
        pMaterial->vDiffuse.z = (float)theColor.mBlue;

        // Display the Emissive
        lKFbxDouble3 =((FbxSurfaceLambert *)lFbxMaterial)->Emissive;
        theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

        pMaterial->vEmissive.x = (float)theColor.mRed;
        pMaterial->vEmissive.y = (float)theColor.mGreen;
        pMaterial->vEmissive.z = (float)theColor.mBlue;

        // Display the Opacity
        lKFbxDouble1 =((FbxSurfaceLambert *)lFbxMaterial)->TransparencyFactor;

        pMaterial->fOpacity = 1.0f-(float)lKFbxDouble1.Get();

        pMaterial->bSpecular = false;
    }            


    FbxProperty lProperty;
    //go through all the possible textures

    lProperty = lFbxMaterial->FindProperty(lFbxMaterial->sDiffuse);
    pMaterial->bDiffuseTexture = GetTextureFile(&lProperty, pMaterial->strDiffuseTexture);
    if (!pMaterial->bDiffuseTexture)
    {
        lProperty = lFbxMaterial->FindProperty(lFbxMaterial->sDiffuseFactor);
        pMaterial->bDiffuseTexture = GetTextureFile(&lProperty, pMaterial->strDiffuseTexture);
    }

    lProperty = lFbxMaterial->FindProperty(lFbxMaterial->sEmissive);
    pMaterial->bEmissiveTexture = GetTextureFile(&lProperty, pMaterial->strEmissiveTexture);

    lProperty = lFbxMaterial->FindProperty(lFbxMaterial->sNormalMap);
    pMaterial->bNormalMap = GetTextureFile(&lProperty, pMaterial->strNormalMap);
}

UINT WEFbxLoader::ParseMaterial(FbxGeometry* pGeometry)
{
    int lMaterialCount = 0;
    FbxNode* lNode = NULL;
    if(pGeometry){
        lNode = pGeometry->GetNode();
        if(lNode)
            lMaterialCount = lNode->GetMaterialCount();    
    }

    if (lMaterialCount > 0)
    {
        FbxPropertyT<FbxDouble3> lKFbxDouble3;
        FbxPropertyT<FbxDouble> lKFbxDouble1;
        FbxColor theColor;

        for (int lCount = 0; lCount < lMaterialCount; lCount ++)
        {
            FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(lCount);

            // Return if the material exist.
            WCHAR mtlNamel[MAX_PATH];
            MultiByteToWideChar(CP_ACP, 0, lMaterial->GetName(), -1, mtlNamel, MAX_PATH);
            for (int j = 0; j < m_arrvMaterial.size(); j++)
            {
                WCHAR* cmpMtlName = m_arrvMaterial[j].strMaterialName;
                if ( 0 == wcscmp(cmpMtlName, mtlNamel))
                {
                    return j;
                }
            }

            // Push back a new material strucure.
            MATERIAL newMaterial;
            FillMaterialStructure(&newMaterial, lMaterial);
            m_arrvMaterial.push_back(newMaterial);

            return m_arrvMaterial.size() - 1;
        }
    }
    return 0;
}

void WEFbxLoader::ParseMesh(FbxMesh* pMesh)
{
    // Load bone data of the control point.
    vector<Vertex_Bone_Weight>* pArrvVBW = LoadBoneWeight(pMesh);

    UINT uPolygonCount = pMesh->GetPolygonCount();

    UINT controlPoint3[3];
    XMFLOAT3 position3[3];
    XMFLOAT3 normal3[3];
    XMFLOAT2 texcoord3[3];
    BYTE4 bone3[3];
    BYTE4 weight3[3];

    UINT uVertexId = 0;
    UINT uTriangleId = 0;

    UINT uMaterialIndex = ParseMaterial(pMesh);

    for (UINT iPolygon = 0; iPolygon < uPolygonCount; iPolygon++)
    {    
        UINT triangleCount = pMesh->GetPolygonSize(iPolygon) - 2;

        for (UINT iTriangle = 0; iTriangle < triangleCount; iTriangle++)
        {

            controlPoint3[0] = GetPosition(pMesh, &position3[0], iPolygon, 0);
            controlPoint3[1] = GetPosition(pMesh, &position3[1], iPolygon, iTriangle + 1);
            controlPoint3[2] = GetPosition(pMesh, &position3[2], iPolygon, iTriangle + 2);

            GetUV(pMesh, &texcoord3[0], iPolygon, 0);
            GetUV(pMesh, &texcoord3[1], iPolygon, iTriangle + 1);
            GetUV(pMesh, &texcoord3[2], iPolygon, iTriangle + 2);

            GetNormal(pMesh, &normal3[0], iPolygon, 0);
            GetNormal(pMesh, &normal3[1], iPolygon, iTriangle + 1);
            GetNormal(pMesh, &normal3[2], iPolygon, iTriangle + 2);

            int lControlPointIndex0 = pMesh->GetPolygonVertex(iPolygon, 0);  
            int lControlPointIndex1 = pMesh->GetPolygonVertex(iPolygon, iTriangle + 1);  
            int lControlPointIndex2 = pMesh->GetPolygonVertex(iPolygon, iTriangle + 2);  
            
            for (UINT i = 0; i < 4; i++)
            {
                bone3[0][i] = (BYTE)(*pArrvVBW)[lControlPointIndex0].bone[i];
                weight3[0][i] = (BYTE)((*pArrvVBW)[lControlPointIndex0].weight[i] * 255);

                bone3[1][i] = (BYTE)(*pArrvVBW)[lControlPointIndex1].bone[i];
                weight3[1][i] = (BYTE)((*pArrvVBW)[lControlPointIndex1].weight[i] * 255);

                bone3[2][i] = (BYTE)(*pArrvVBW)[lControlPointIndex2].bone[i];
                weight3[2][i] = (BYTE)((*pArrvVBW)[lControlPointIndex2].weight[i] * 255);
            }

            // Push back a triangle
            for (int i = 0; i < 3; i++)
            {
                m_arrvIdxControlPoint.push_back(controlPoint3[i]);
                m_arrvPosition.push_back(position3[i]);
                m_arrvNormal.push_back(normal3[i]);
                m_arrvTexcoord.push_back(texcoord3[i]);
                m_arrvBone.push_back(bone3[i]);
                m_arrvWeight.push_back(weight3[i]);
            }
            m_arrvTriangleMaterialIndex.push_back(uMaterialIndex);
        } // for polygon triangle count
    } // for polygonCount

    SAFE_DELETE(pArrvVBW);
}

void WEFbxLoader::LoadMesh(FbxNode* pNode)
{
    FbxNodeAttribute::EType lAttributeType;
    int i;

    if(pNode->GetNodeAttribute() != NULL)
    {
        lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

        switch (lAttributeType)
        {  
        case FbxNodeAttribute::eMesh:      
            ParseMesh(pNode->GetMesh());
            break;
        }   
    }

    for(i = 0; i < pNode->GetChildCount(); i++)
    {
        LoadMesh(pNode->GetChild(i));
    }
}

UINT WEFbxLoader::GetVertex(VERTEX_STATIC* pVertex, UINT idx)
{
    pVertex->position = m_arrvPosition[idx];
    pVertex->normal = m_arrvNormal[idx];
    pVertex->texcoord = m_arrvTexcoord[idx];

    return m_arrvIdxControlPoint[idx];
}

UINT WEFbxLoader::GetVertex(VERTEX_SKELETON* pVertex, UINT idx)
{
    pVertex->position = m_arrvPosition[idx];
    pVertex->normal = m_arrvNormal[idx];
    pVertex->texcoord = m_arrvTexcoord[idx];

    for (int i = 0; i < 4; i++)
    {
        pVertex->weight[i] = m_arrvWeight[idx][i];
        pVertex->bone[i] = m_arrvBone[idx][i];
    }

    return m_arrvIdxControlPoint[idx];
}