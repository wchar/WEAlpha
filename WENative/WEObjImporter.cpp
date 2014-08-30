#include "WEObjImporter.h"
#include <fstream>

///------------------------------------------------------------
// WEObjImporter
///------------------------------------------------------------

WEObjImporter::WEObjImporter(const char* cstrFileName)
{
    HRESULT hr;
    WCHAR wstr[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, cstrFileName, -1, wstr, MAX_PATH);
    V(LoadGeometryFromOBJ(wstr));
}

WEObjImporter::WEObjImporter(const WCHAR* wstrFileName)
{
    HRESULT hr;
    V(LoadGeometryFromOBJ(wstrFileName));
}

WEObjImporter::~WEObjImporter()
{
    for (UINT iMaterial = 0; iMaterial < m_Materials.size(); ++iMaterial)
    {
        MATERIAL *pMaterial = m_Materials.at(iMaterial);
        SAFE_DELETE(pMaterial);
    }
    m_Materials.clear();
    m_Vertices.clear();
    m_Indices.clear();
    m_Attributes.clear();
}

HRESULT WEObjImporter::LoadGeometryFromOBJ(const WCHAR* strFileName)
{
    WCHAR strMaterialFilename[MAX_PATH] = {0};
    WCHAR wstr[MAX_PATH];
    char str[MAX_PATH];
    HRESULT hr;

    // Find the file
    wcscpy_s(wstr, strFileName);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, MAX_PATH, NULL, NULL);

    // Store the directory where the mesh was found
    wcscpy_s(m_strMediaDir, MAX_PATH - 1, wstr);
    WCHAR* pch = wcsrchr(m_strMediaDir, L'\\');
    if (pch)
        *pch = NULL;

    // Create temporary storage for the input data
    vector <XMFLOAT3> Positions;
    vector <XMFLOAT2> TexCoords;
    vector <XMFLOAT3> Normals;

    // The first subset uses the default material
    MATERIAL* pMaterial = new MATERIAL();
    if (pMaterial == NULL)
        return E_OUTOFMEMORY;

    InitMaterial(pMaterial);
    wcscpy_s(pMaterial->strMaterialName, MAX_PATH - 1, L"default");
    m_Materials.push_back(pMaterial);

    DWORD dwCurSubset = 0;

    // File input
    WCHAR strCommand[256] = {0};
    wifstream InFile(strFileName);
    if (!InFile)
        return DXTRACE_ERR(L"wifstream::open", E_FAIL);

    while (true)
    {
        InFile >> strCommand;
        if (!InFile)
            break;

        if (0 == wcscmp(strCommand, L"#"))
        {
            // Comment
        }
        else if (0 == wcscmp(strCommand, L"v"))
        {
            // Vertex Position
            float x, y, z;
            InFile >> x >> y >> z;
            Positions.push_back(XMFLOAT3(x, y, z));
        }
        else if (0 == wcscmp(strCommand, L"vt"))
        {
            // Vertex TexCoord
            float u, v;
            InFile >> u >> v;
            TexCoords.push_back(XMFLOAT2(u, v));
        }
        else if (0 == wcscmp(strCommand, L"vn"))
        {
            // Vertex Normal
            float x, y, z;
            InFile >> x >> y >> z;
            Normals.push_back(XMFLOAT3(x, y, z));
        }
        else if (0 == wcscmp(strCommand, L"f"))
        {
            // Face
            UINT iPosition, iTexCoord, iNormal;
            VERTEX vertex;

            for (UINT iFace = 0; iFace < 3; iFace++)
            {
                ZeroMemory(&vertex, sizeof(VERTEX));

                // OBJ format uses 1-based arrays
                InFile >> iPosition;
                vertex.position = Positions[iPosition - 1];

                if ('/' == InFile.peek())
                {
                    InFile.ignore();

                    if ('/' != InFile.peek())
                    {
                        // Optional texture coordinate
                        InFile >> iTexCoord;
                        vertex.texcoord = TexCoords[iTexCoord - 1];
                    }

                    if ('/' == InFile.peek())
                    {
                        InFile.ignore();

                        // Optional vertex normal
                        InFile >> iNormal;
                        vertex.normal = Normals[iNormal - 1];
                    }
                }

                // If a duplicate vertex doesn't exist, add this vertex to the Vertices
                // list. Store the index in the Indices array. The Vertices and Indices
                // lists will eventually become the Vertex Buffer and Index Buffer for
                // the mesh.
                DWORD index = AddVertex(iPosition, &vertex);
                if (index == (DWORD)-1)
                    return E_OUTOFMEMORY;

                m_Indices.push_back(index);
            }
            m_Attributes.push_back(dwCurSubset);
        }
        else if (0 == wcscmp(strCommand, L"mtllib"))
        {
            // Material library
            InFile >> strMaterialFilename;
        }
        else if (0 == wcscmp(strCommand, L"usemtl"))
        {
            // Material
            WCHAR strName[MAX_PATH] = {0};
            InFile >> strName;

            bool bFound = false;
            for (UINT iMaterial = 0; iMaterial < m_Materials.size(); iMaterial++)
            {
                MATERIAL* pCurMaterial = m_Materials.at(iMaterial);
                if (0 == wcscmp(pCurMaterial->strMaterialName, strName))
                {
                    bFound = true;
                    dwCurSubset = iMaterial;
                    break;
                }
            }

            if (!bFound)
            {
                pMaterial = new MATERIAL();
                if (pMaterial == NULL)
                    return E_OUTOFMEMORY;

                dwCurSubset = m_Materials.size();

                InitMaterial(pMaterial);
                wcscpy_s(pMaterial->strMaterialName, MAX_PATH - 1, strName);

                m_Materials.push_back(pMaterial);
            }
        }
        else
        {
            // Unimplemented or unrecognized command
        }

        InFile.ignore(1000, '\n');
    }

    // Cleanup
    InFile.close();
    DeleteCache();

    // If an associated material file was found, read that in as well.
    if (strMaterialFilename[0])
    {
        V_RETURN(LoadMaterialsFromMTL(strMaterialFilename));
    }

    return S_OK;

}

DWORD WEObjImporter::AddVertex(UINT hash, VERTEX* pVertex)
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
            VERTEX* pCacheVertex = m_Vertices.data() + pEntry->index;

            // If this vertex is identical to the vertex already in the list, simply
            // point the index buffer to the existing vertex
            if (0 == memcmp(pVertex, pCacheVertex, sizeof(VERTEX)))
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
        index = m_Vertices.size();
        m_Vertices.push_back(*pVertex);

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

void WEObjImporter::DeleteCache()
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

HRESULT WEObjImporter::LoadMaterialsFromMTL(const WCHAR* strFileName)
{
    // Set the current directory based on where the mesh was found
    WCHAR wstrOldDir[MAX_PATH] = {0};
    GetCurrentDirectory(MAX_PATH, wstrOldDir);
    SetCurrentDirectory(m_strMediaDir);

    // Find the file
    WCHAR str[MAX_PATH];
    char cstr[MAX_PATH];

    // Find mtl file
    wcscpy_s(str, m_strMediaDir);
    wcscat_s(str, L"\\");
    wcscat_s(str, strFileName);
    WideCharToMultiByte(CP_ACP, 0, str, -1, cstr, MAX_PATH, NULL, NULL);

    // Restore the original current directory
    SetCurrentDirectory(wstrOldDir);

    // File input 
    WCHAR strCommand[256] = {0};
    wifstream InFile(cstr);
    if (!InFile)
        return DXTRACE_ERR(L"wifstream::open", E_FAIL);

    MATERIAL* pMaterial = NULL;

    for (; ;)
    {
        InFile >> strCommand;
        if (!InFile)
            break;

        if (0 == wcscmp(strCommand, L"newmtl"))
        {
            // Switching active materials
            WCHAR strName[MAX_PATH] = {0};
            InFile >> strName;

            pMaterial = NULL;
            for (UINT i = 0; i < m_Materials.size(); i++)
            {
                MATERIAL* pCurMaterial = m_Materials.at(i);
                if (0 == wcscmp(pCurMaterial->strMaterialName, strName))
                {
                    pMaterial = pCurMaterial;
                    break;
                }
            }
        }

        // The rest of the commands rely on an active material
        if (pMaterial == NULL)
            continue;

        if (0 == wcscmp(strCommand, L"#"))
        {
            // Comment
        }
        else if (0 == wcscmp(strCommand, L"Ka"))
        {
            // Ambient color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vAmbient = XMFLOAT3(r, g, b);
        }
        else if (0 == wcscmp(strCommand, L"Kd"))
        {
            // Diffuse color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vDiffuse = XMFLOAT3(r, g, b);
        }
        else if (0 == wcscmp(strCommand, L"Ks"))
        {
            // Specular color
            float r, g, b;
            InFile >> r >> g >> b;
            pMaterial->vSpecular = XMFLOAT3(r, g, b);
        }
        else if (0 == wcscmp(strCommand, L"d") ||
            0 == wcscmp(strCommand, L"Tr"))
        {
            // Alpha
            InFile >> pMaterial->fOpacity;
        }
        else if (0 == wcscmp(strCommand, L"Ns"))
        {
            // Shininess
            int nShininess;
            InFile >> nShininess;
            pMaterial->fShininess = nShininess;
        }
        else if (0 == wcscmp(strCommand, L"illum"))
        {
            // Specular on/off
            int illumination;
            InFile >> illumination;
            pMaterial->bSpecular = (illumination == 2);
        }
        else if (0 == wcscmp(strCommand, L"map_Kd"))
        {
            // Texture
            InFile >> str;
            wcscpy_s(pMaterial->strDiffuseTexture, m_strMediaDir);
            wcscat_s(pMaterial->strDiffuseTexture, L"\\");
            wcscat_s(pMaterial->strDiffuseTexture, str);
            pMaterial->bDiffuseTexture = true;
        }
        else
        {
            // Unimplemented or unrecognized command
        }

        InFile.ignore(1000, L'\n');
    }

    InFile.close();


}

void WEObjImporter::InitMaterial(MATERIAL* pMaterial)
{
    ZeroMemory(pMaterial, sizeof(MATERIAL));

    pMaterial->vAmbient = XMFLOAT3(0.2f, 0.2f, 0.2f);
    pMaterial->vDiffuse = XMFLOAT3(0.8f, 0.8f, 0.8f);
    pMaterial->vSpecular = XMFLOAT3(1.0f, 1.0f, 1.0f);
    pMaterial->fShininess = 0.0f;
    pMaterial->fOpacity = 1.0f;
}

