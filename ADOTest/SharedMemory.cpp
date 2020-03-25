// SharedMemory.cpp: implementation of the CSharedMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SharedMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSharedMemory, CObject)

CSharedMemory::CSharedMemory()
    : m_pvData(NULL)
    , m_hMutex(0)
    , m_hMapping(0)
    , m_dwLength(0)
    , m_bOpen(FALSE)
    , m_bUnMappedOnDeleted(true)
{
}

CSharedMemory::CSharedMemory(LPCTSTR pszName, DWORD dwBytes, bool bNameExtension)
    : m_pvData(NULL)
    , m_hMutex(0)
    , m_hMapping(0)
    , m_dwLength(0)
    , m_bOpen(FALSE)
    , m_bUnMappedOnDeleted(true)
{
    CreateSharedMemEx(pszName, dwBytes, bNameExtension);
}

CSharedMemory::~CSharedMemory()
{
    if (m_hMutex)
    {
        ReleaseMutex(m_hMutex);
        CloseHandle(m_hMutex);
    }

    if (m_bUnMappedOnDeleted
        && m_pvData)
    {
        UnmapViewOfFile(m_pvData);

        if (m_hMapping)
        {
            CloseHandle(m_hMapping);
        }
    }
}

// return 0 or ERROR_ALREADY_EXISTS if it already exists.
DWORD CSharedMemory::CreateSharedMem(LPCTSTR pszName, DWORD dwBytes)
{
    return CreateSharedMemEx(pszName, dwBytes, true);
}

// methods to set whether to ummap shm upon destruction
// default is auto unmap
void CSharedMemory::SetUnmappedOnDeleted(bool bDelete)
{
    m_bUnMappedOnDeleted =  bDelete;
}

// round up to nearest page size block
DWORD CSharedMemory::CalculateShmSize(DWORD dwSize)
{
    SYSTEM_INFO sysInfo;

    GetSystemInfo(&sysInfo);
    long page_size = sysInfo.dwPageSize;
    // round up to next 4096 block (PageSize)
    DWORD dwShmSize = ((dwSize / page_size) + (dwSize % page_size ? 1 : 0)) * page_size;
    //
    return dwShmSize;
}

// 
DWORD CSharedMemory::GetSize()
{
    if (m_hMapping == NULL)
        return 0;

    MEMORY_BASIC_INFORMATION info;
    SIZE_T BufferSize = ::VirtualQueryEx(::GetCurrentProcess(), m_pvData, &info, sizeof(info));

    if (BufferSize) // byte size of info structure
        return info.RegionSize;

    return 0;
}

// return 0 or ERROR_ALREADY_EXISTS if it already exists.
DWORD CSharedMemory::CreateSharedMemEx(LPCTSTR pszName, DWORD dwBytes, bool bNameExtension)
{
    CString csName(pszName);

    if (bNameExtension)
    {
        csName            += "_FM";
    }

    DWORD dwShmSize = CalculateShmSize(dwBytes);

    m_hMapping = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, dwShmSize, (LPCTSTR)csName);

    if (m_hMapping)
    {
        m_pvData   = MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        m_dwLength = dwBytes;
        m_hMutex   = CreateMutex(NULL, FALSE, LPCTSTR(csName + "_MX"));
    }

    return (DWORD)m_hMapping;
}

LPVOID CSharedMemory::getDirectMap()
{
    return m_pvData;
}

// must call open to get access to memory
// return the void* pointer if successful, user needs to tyoe cast the pointer to use
// memory properly
// return NULL if open is unsuccessful
LPVOID CSharedMemory::Open(DWORD dwTimeOut /*=INFINITE*/)
{
    if (m_hMapping) // open successfully
    {
        if (WaitForSingleObject(m_hMutex, dwTimeOut) == WAIT_OBJECT_0)
        {
            m_bOpen = TRUE;
            return m_pvData;
        }
    }

    return NULL;
}

// release the memory for others to use
BOOL CSharedMemory::Close()
{
    m_bOpen = FALSE;
    return ReleaseMutex(m_hMutex);
}

#ifdef _DEBUG
void CSharedMemory::AssertValid() const
{
    CObject::AssertValid();
    ASSERT(m_dwLength > 0);
    ASSERT(m_hMapping);
    ASSERT(m_pvData);
    ASSERT(m_hMutex);
}

void CSharedMemory::Dump(CDumpContext &dc) const
{
    CObject::Dump(dc);
    dc << "Length = " << m_dwLength << "\n";
    dc << "m_hMapping = " << m_hMapping << "\n";
    dc << "m_pvData = " << m_pvData << "\n";
    dc << "m_hMutex = " << m_hMutex << "\n";
}

#endif // _DEBUG
