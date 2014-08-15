#include "WEContent.h"

WEContent::WEContent()
{
    m_pStaticMemory = NULL;
    m_uSize = 0;
}

WEContent::~WEContent()
{
    SAFE_DELETE(m_pStaticMemory);
}

bool WEContent::Load(WCHAR* wstrFile)
{
    // open file
    HANDLE hFile = CreateFile(wstrFile, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if( INVALID_HANDLE_VALUE == hFile )
        return false;

    // get size of the file
    LARGE_INTEGER FileSize;
    GetFileSizeEx( hFile, &FileSize );
    m_uSize = FileSize.LowPart;

    // allocate memory
    m_pStaticMemory = new BYTE[m_uSize];

    // read file
    DWORD dwBytesRead;
    if( !ReadFile( hFile, m_pStaticMemory, m_uSize, &dwBytesRead, NULL ) )
        return false;

    // close file
    CloseHandle( hFile );

    // create content
    if (!FromContentBuffer())
    {
        SAFE_DELETE(m_pStaticMemory);
        return false;
    };

    return true;
}

bool WEContent::Save(WCHAR* wstrFile)
{   
    // create
    if (!m_pStaticMemory)
        return false;

    if (!wstrFile)
        wstrFile = m_wstrFileLocation;
    // open file
    HANDLE hFile = CreateFile(wstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if( hFile == INVALID_HANDLE_VALUE )
    {
        return false;
    }
    // write file
    DWORD dwBytesWritten;
    WriteFile( hFile, m_pStaticMemory, m_uSize, &dwBytesWritten, NULL );
    // close file
    CloseHandle( hFile );

    return true;
}

bool WEContent::Create(BYTE* pBuf, UINT uSize)
{
    m_pStaticMemory = new BYTE[uSize];
    m_uSize = uSize;
    // copy
    CopyMemory(m_pStaticMemory, pBuf, uSize);

    // try to create with function
    if (FromContentBuffer())
        return true;

    // failed
    SAFE_DELETE(m_pStaticMemory);
    return false;
}