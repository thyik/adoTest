// SharedMemory.h: interface for the CSharedMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREDMEMORY_H__D6015341_E3F1_11D7_89BD_000629A6E157__INCLUDED_)
#define AFX_SHAREDMEMORY_H__D6015341_E3F1_11D7_89BD_000629A6E157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef  BUILD_MCCTRDLL
#define     UTILITYDLL   __declspec(dllexport)
#else
#define     UTILITYDLL   __declspec(dllimport)
#endif

#include <afx.h>

class /*UTILITYDLL*/ CSharedMemory : public CObject
{
public:
    CSharedMemory();
    CSharedMemory(LPCTSTR pszName, DWORD dwBytes, bool NameExtension);
    virtual ~CSharedMemory();

    DWORD  CreateSharedMem(LPCTSTR pszName, DWORD dwBytes);
    DWORD  CreateSharedMemEx(LPCTSTR pszName, DWORD dwBytes,  bool NameExtension = false);
    LPVOID getDirectMap();
    
    DWORD GetSize();
    //
    // methods to set whether to ummap shm upon destruction
    void SetUnmappedOnDeleted(bool bDelete);

    // Will return 0 or ERROR_ALREADY_EXISTS if it already exists.
    LPVOID Open(DWORD dwTimeout = INFINITE); // SM must be opened before use. The pointer
                                             // returns must be type casted to it type
                                             // Returns NULL if the SM was not created ot failed
    BOOL Close();                            // SM should be closed after use so that others can open the SM
    BOOL    IsOpen()
    {
        return m_bOpen;
    }                                        // Check if SM is opened

    DWORD   GetLength()
    {
        return m_dwLength;
    }

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_DYNAMIC(CSharedMemory)

private:
    DWORD CalculateShmSize(DWORD dwSize);

    HANDLE m_hMutex;
    LPVOID m_pvData;
    HANDLE m_hMapping;
    BOOL   m_bOpen;
    DWORD  m_dwLength;
    bool   m_bUnMappedOnDeleted;

};

#endif // !defined(AFX_SHAREDMEMORY_H__D6015341_E3F1_11D7_89BD_000629A6E157__INCLUDED_)
