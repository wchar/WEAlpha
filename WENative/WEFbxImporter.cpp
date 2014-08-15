#include "WEFbxImporter.h"
#include "WEFbxLoader.h"

//---------------------------------------------------------------------
// Class WEFbxImporter
//---------------------------------------------------------------------
WEFbxImporter::WEFbxImporter(WCHAR* strFileName)
{
    m_bResult = false;
    Create(strFileName);
}

WEFbxImporter::~WEFbxImporter()
{

}

void WEFbxImporter::Create(WCHAR* strFileName)
{
    WEFbxLoader fbxLoader(strFileName);

    m_bResult = fbxLoader.m_bResult;
   
    if (m_bResult)// Load successfully
    {
        UINT materialCount = fbxLoader.m_arrvMaterial.size();
        VERTEX_STATIC vertex;
        VERTEX_SKELETON vertex_s;
        UINT iPosition, vertexIndex, idxVertex;
        MESH_SUBSET subset;
        for (int i = 0; i < materialCount; i++)
        {            
            subset.IndexCount = 0;
            subset.IndexStart = m_Indices.size();
            subset.MaterialID = i;
            subset.VertexStart = 0;

            for (int j = 0; j < fbxLoader.m_arrvTriangleMaterialIndex.size(); j++)            
            {
                if (fbxLoader.m_arrvTriangleMaterialIndex[j] == i)
                {
                    idxVertex = j * 3;
                    // Triangle corner0
                    fbxLoader.GetVertex(&vertex_s, idxVertex + 0);
                    iPosition = fbxLoader.GetVertex(&vertex, idxVertex + 0);
                    vertexIndex = AddVertex(iPosition, &vertex, &vertex_s);
                    m_Indices.push_back(vertexIndex);

                    // Triangle corner1
                    fbxLoader.GetVertex(&vertex_s, idxVertex + 1);
                    iPosition = fbxLoader.GetVertex(&vertex, idxVertex + 1);
                    vertexIndex = AddVertex(iPosition, &vertex, &vertex_s);
                    m_Indices.push_back(vertexIndex);

                    // Triangle corner2
                    fbxLoader.GetVertex(&vertex_s, idxVertex + 2);
                    iPosition = fbxLoader.GetVertex(&vertex, idxVertex + 2);
                    vertexIndex = AddVertex(iPosition, &vertex, &vertex_s);
                    m_Indices.push_back(vertexIndex);
                
                    subset.IndexCount += 3;
                }
            }

            m_Materials.push_back(fbxLoader.m_arrvMaterial[i]);

            m_Subsets.push_back(subset);
        }

        // Anim
        m_AnimtationInfo.uJointCount = fbxLoader.m_arrvJoint.size();

        UINT sizeofdata = m_AnimtationInfo.uJointCount * sizeof(JOINT);
        CopyMemory(m_AnimtationInfo.mJoints, fbxLoader.m_arrvJoint.data(), sizeofdata);

        m_AnimtationInfo.uFrameCount = fbxLoader.m_uNumFrame;
        m_AnimtationInfo.uFramePreSecond = fbxLoader.m_uFramePreSecond;
        
        UINT numGlobals = m_AnimtationInfo.uJointCount * m_AnimtationInfo.uFrameCount;
        sizeofdata = numGlobals * sizeof(XMFLOAT4X4);
        m_AnimtationInfo.mGlobals = new XMFLOAT4X4[numGlobals];
        CopyMemory(m_AnimtationInfo.mGlobals, fbxLoader.m_arrvGlobalMatrix.data(), sizeofdata);
    }

    ReleaseCacheEntry();
}

void WEFbxImporter::ReleaseCacheEntry()
{
    // Iterate through all the elements in the cache and subsequent linked lists
    for (UINT i = 0; i < m_VertexCache.size(); i++)
    {
        CacheEntry* pEntry = m_VertexCache.at(i);
        while(pEntry != NULL)
        {
            CacheEntry* pNext = pEntry->pNext;
            SAFE_DELETE(pEntry);
            pEntry = pNext;
        }
    }

    m_VertexCache.clear();
}

DWORD WEFbxImporter::AddVertex(UINT hash, VERTEX_STATIC* pVertex, VERTEX_SKELETON* pVertex_S)
{
    // If this vertex doesn't already exist in the Vertices list, create a new entry.
    // Add the index of the vertex to the Indices list.
    bool bFoundInList = false;
    DWORD index = 0;

    // Since it's very slow to check every element in the vertex list, a hashtable stores
    // vertex indices according to the vertex position's index as reported by the OBJ file
    if ((UINT)m_VertexCache.size() > hash)
    {
        CacheEntry* pEntry = m_VertexCache.at(hash);
        while(pEntry != NULL)
        {
            VERTEX_STATIC* pCacheVertex = m_StaticVertices.data() + pEntry->index;

            // If this vertex is identical to the vertex already in the list, simply
            // point the index buffer to the existing vertex
            if (0 == memcmp(pVertex, pCacheVertex, sizeof(VERTEX_STATIC)))
            {
                bFoundInList = true;
                index = pEntry->index;
                break;
            }

            pEntry = pEntry->pNext;
        }
    }

    // Vertex was not found in the list. Create a new entry, both within the Vertices list
    // and also within the hashtable cache
    if (!bFoundInList)
    {
        // Add to the Vertices list
        index = m_StaticVertices.size();
        m_StaticVertices.push_back(*pVertex);
        m_SkeletonVertices.push_back(*pVertex_S);

        // Add this to the hashtable
        CacheEntry* pNewEntry = new CacheEntry;
        if (pNewEntry == NULL)
            return (DWORD)-1;

        pNewEntry->index = index;
        pNewEntry->pNext = NULL;

        // Grow the cache if needed
        while((UINT)m_VertexCache.size() <= hash)
        {
            m_VertexCache.push_back(NULL);
        }

        // Add to the end of the linked list
        CacheEntry* pCurEntry = m_VertexCache.at(hash);
        if (pCurEntry == NULL)
        {
            // This is the head element
            m_VertexCache[hash] = pNewEntry;
        }
        else
        {
            // Find the tail
            while(pCurEntry->pNext != NULL)
            {
                pCurEntry = pCurEntry->pNext;
            }

            pCurEntry->pNext = pNewEntry;
        }
    }

    return index;
}