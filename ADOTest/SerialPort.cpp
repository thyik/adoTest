// =============================================================================
// File    : SerialPort.cpp
// Author  : Eric Woodruff,  CIS ID: 72134,1150
// Updated : Mon 04/07/97 21:28:30
// Note    : Copyright 1996-97, Eric Woodruff, All rights reserved
// Compiler: Borland C++ 5.xx, Visual C++ 4.xx
//
// This file contains the member functions for CSerialPort.  This class
// is a wrapper for the Win32 serial communication API functions.
//
// This class can be compiled for a DLL.  Use the appropriate #define:
//  BUILD_SPCOMM_DLL defined   - Build DLL
//  BUILD_SPCOMM_NODLL defined - Not a DLL
//  Neither defined            - Assumed to be an #include for app DLL import
//
// =============================================================================
#include "stdafx.h"
#include <windows.h>
#include <winioctl.h>

#include <string.h>
#include <algorithm>

#include "SerialPort.h"

// =============================================================================
// Constructor and destructor
// =============================================================================

CSerialPort::CSerialPort(const char *szPortName)
{
    // Clear data members
    hPortId    = INVALID_HANDLE_VALUE;
    hThread    = NULL;
    dwThreadId = dwLastError = dwCommErrors = dwEventMask = 0;

    memset(&olRead, 0, sizeof(OVERLAPPED));
    memset(&olWrite, 0, sizeof(OVERLAPPED));
    memset(&olWait, 0, sizeof(OVERLAPPED));

    // Open the port.  The device must exist and must be opened for
    // exclusive access (no sharing).  No security is used, overlapped
    // I/O is enabled.
    hPortId = CreateFile(szPortName, GENERIC_READ | GENERIC_WRITE,
                         0, NULL, OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH |
                         FILE_FLAG_OVERLAPPED, NULL);

    if (hPortId != INVALID_HANDLE_VALUE)
    {
        // Create the overlapped event structures.  No security, explicit
        // reset request, initial event reset, no name.
        olRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (olRead.hEvent)
        {
            olWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            if (olWrite.hEvent)
            {
                olWait.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            }
        }

        if (olRead.hEvent && olWrite.hEvent && olWait.hEvent)
        {
            // Okay, everything is open, set up some default parameters:
            // 19200, N81, hardware flow control (DTR/DSR, RTS/CTS),
            // 4K I/O buffers, no read timeout (immediate return), 1
            // second write timeout, no LSR/MST in the regular data stream
            SetBaudRate();
            SetParityDataStop();
            SetFlowControl();
            SetBufferSizes();
            SetReadTimeouts();
            SetWriteTimeouts();
            LSRMST_INSERT(0);

            // Set it up to receive data by default and clear any
            // stray data that may be hanging around.
            ::SetCommMask(hPortId, EV_RXCHAR);
            ::PurgeComm(hPortId, PURGE_TXABORT | PURGE_RXABORT |
                        PURGE_TXCLEAR | PURGE_RXCLEAR);
        }
        else
        {
            dwLastError = ::GetLastError();
        }
    }
    else
    {
        dwLastError = ::GetLastError();
    }
}

CSerialPort::~CSerialPort()
{
    if (hPortId != INVALID_HANDLE_VALUE)
    {
        // Stop any running comm thread
        StopCommThread();

        // Purge any outstanding reads/writes
        ::PurgeComm(hPortId, PURGE_TXABORT | PURGE_RXABORT |
                    PURGE_TXCLEAR | PURGE_RXCLEAR);

        // Dispose of the overlapped I/O structures and close the port
        if (olRead.hEvent)
        {
            CloseHandle(olRead.hEvent);
        }

        if (olWrite.hEvent)
        {
            CloseHandle(olWrite.hEvent);
        }

        if (olWait.hEvent)
        {
            CloseHandle(olWait.hEvent);
        }

        CloseHandle(hPortId);
    }
}

// =============================================================================
// Initialization/configuration functions.  These all set various options
// for device I/O.  Most are self-explanatory.
// =============================================================================

BOOL CSerialPort::SetBaudRate(long lBaudRate)
{
    DCB  dcb;
    BOOL bRetVal;

    dwLastError = ERROR_SUCCESS;

    // Always retrieve the current state before modifying it
    dcb.DCBlength = sizeof(DCB);
    bRetVal       = ::GetCommState(hPortId, &dcb);

    if (bRetVal)
    {
        dcb.BaudRate = lBaudRate; //  nBaudRate;
        bRetVal      = ::SetCommState(hPortId, &dcb);
    }

    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::SetParityDataStop(int nParity, int nDataBits,
                                    int nStopBits)
{
    DCB  dcb;
    BOOL bRetVal;

    dwLastError = ERROR_SUCCESS;

    dcb.DCBlength = sizeof(DCB);
    bRetVal       = ::GetCommState(hPortId, &dcb);

    if (bRetVal)
    {
        dcb.Parity   = static_cast<BYTE>(nParity);
        dcb.ByteSize = static_cast<BYTE>(nDataBits);
        dcb.StopBits = static_cast<BYTE>(nStopBits);
        dcb.fParity  = TRUE; // Parity errors are reported

        // Enable the options
        bRetVal = ::SetCommState(hPortId, &dcb);
    }

    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

// Setting flow control can get a little involved.  This function takes
// care of all the necessary details based upon the parameters passed.
BOOL CSerialPort::SetFlowControl(int nFlowCtrl, int nXOnLimit,
                                 int nXOffLimit, char chXOnChar, char chXOffChar)
{
    DCB dcb;

    dwLastError = ERROR_SUCCESS;

    dcb.DCBlength = sizeof(DCB);
    BOOL bRetVal = ::GetCommState(hPortId, &dcb);

    if (bRetVal)
    {
        // Set hardware flow control options
        BYTE byOnOff = (BYTE)((nFlowCtrl & PCF_DTRDSR) != 0);
        dcb.fOutxDsrFlow = byOnOff;
        if (byOnOff)
        {
            dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
        }
        else
        {
            dcb.fDtrControl = DTR_CONTROL_ENABLE;
        }

        byOnOff          = (BYTE)((nFlowCtrl & PCF_RTSCTS) != 0);
        dcb.fOutxCtsFlow = byOnOff;
        if (byOnOff)
        {
            dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        }
        else
        {
            dcb.fRtsControl = RTS_CONTROL_ENABLE;
        }

        // Set software flow control options
        byOnOff  = (BYTE)((nFlowCtrl & PCF_XONXOFF) != 0);
        dcb.fInX = dcb.fOutX = byOnOff;

        dcb.XonChar  = chXOnChar;
        dcb.XoffChar = chXOffChar;
        dcb.XonLim   = static_cast<WORD>(nXOnLimit);
        dcb.XoffLim  = static_cast<WORD>(nXOffLimit);

        // Miscellaneous
        dcb.fBinary = TRUE;  // Must always be TRUE for NT
        dcb.fNull   = FALSE; // Keep received null bytes

        // Enable the options
        bRetVal = ::SetCommState(hPortId, &dcb);
    }

    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::SetBufferSizes(int nInBufSize, int nOutBufSize)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::SetupComm(hPortId, nInBufSize, nOutBufSize);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::SetReadTimeouts(int nInterval, int nMultiplier,
                                  int nConstant)
{
    COMMTIMEOUTS CommTimeouts;
    BOOL         bRetVal;

    dwLastError = ERROR_SUCCESS;

    // Set the read timeout values
    bRetVal = ::GetCommTimeouts(hPortId, &CommTimeouts);
    if (bRetVal)
    {
        CommTimeouts.ReadIntervalTimeout        = nInterval;
        CommTimeouts.ReadTotalTimeoutMultiplier = nMultiplier;
        CommTimeouts.ReadTotalTimeoutConstant   = nConstant;
        bRetVal                                 = ::SetCommTimeouts(hPortId, &CommTimeouts);
    }

    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::SetWriteTimeouts(int nMultiplier, int nConstant)
{
    COMMTIMEOUTS CommTimeouts;
    BOOL         bRetVal;

    dwLastError = ERROR_SUCCESS;

    // Set the write timeout values
    bRetVal = ::GetCommTimeouts(hPortId, &CommTimeouts);
    if (bRetVal)
    {
        CommTimeouts.WriteTotalTimeoutMultiplier = nMultiplier;
        CommTimeouts.WriteTotalTimeoutConstant   = nConstant;
        bRetVal                                  = ::SetCommTimeouts(hPortId, &CommTimeouts);
    }

    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

// =============================================================================
// Start a thread to handle serial communication
// =============================================================================
BOOL CSerialPort::StartCommThread(LPTHREAD_START_ROUTINE CommProc,
                                  void *pvData)
{
    if (hThread)
    {
        // A thread is already active for this object
        dwLastError = ERROR_ALREADY_EXISTS;
        return FALSE;
    }

    dwLastError = ERROR_SUCCESS;

    // If pvData is NULL, at least pass a pointer to the object.  If
    // pvData refers to a structure, make sure a pointer to the object
    // is included in it or your thread won't have access to the port.
    if (!pvData)
    {
        pvData = this;
    }

    // Create a separate thread to send and/or receive data
    hThread = CreateThread(NULL,
                           0, CommProc, pvData, NORMAL_PRIORITY_CLASS, &dwThreadId);

    if (!hThread)
    {
        dwLastError = ::GetLastError();
    }

    return (BOOL)(hThread != NULL);
}

// =============================================================================
// Stop the comm thread if one is running
// =============================================================================
DWORD CSerialPort::StopCommThread(void)
{
    DWORD dwExitCode;

    dwLastError = dwExitCode = ERROR_SUCCESS;

    if (hThread)
    {
        // Disable event notification and wait for the thread to halt.
        // Clearing the comm mask will generate a comm event that
        // receives a mask value equal to zero.  A thread can use
        // that as an indicator that the port is closing.
        ::SetCommMask(hPortId, 0);

        // Wait for it to exit.
        while (GetExitCodeThread(hThread, &dwExitCode) &&
               dwExitCode == STILL_ACTIVE)
            Sleep(0L);

        CloseHandle(hThread);
        hThread    = NULL;
        dwThreadId = 0;
    }

    return dwExitCode;
}

// =============================================================================
// Handler for received data.  Does nothing by default.
// =============================================================================
int CSerialPort::ProcessData(char *, int)
{
    return 0;
}

// =============================================================================

// Read a block of data from the port.  If bExactSize is TRUE, it will
// attempt to read the full byte count.  If bExactSize is FALSE, it will
// read whatever is available and return immediately.  Note that read timeouts
// must be in effect for bExactSize = TRUE to work properly.
int CSerialPort::ReadCommBlock(LPSTR lpBytes, int nBytesToRead,
                               BOOL bExactSize)
{
    COMSTAT ComStat;
    DWORD   dwBytesRead = 0;

    // Return zero if handle is invalid
    if (hPortId == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    // Read max length or only what is available
    ::ClearCommError(hPortId, &dwCommErrors, &ComStat);

    // If not requiring an exact byte count, get whatever is available
    if (!bExactSize && nBytesToRead > (int)ComStat.cbInQue)
    {
        nBytesToRead = ComStat.cbInQue;
    }

    if (nBytesToRead > 0)
    {
        if (!ReadFile(hPortId, lpBytes, nBytesToRead, &dwBytesRead, &olRead))
        {
            // Did it fail because I/O is still pending?
            dwLastError = ::GetLastError();
            if (dwLastError == ERROR_IO_PENDING)
            {
                // Wait for read to complete or a timeout.
                // NOTE: You can set the last parameter to FALSE for no
                //       waiting, but it tends to consume a lot of CPU
                //       cycles which isn't very friendly.
                while (!GetOverlappedResult(hPortId, &olRead, &dwBytesRead,
                                            TRUE))
                {
                    dwLastError = ::GetLastError();
                    if (dwLastError != ERROR_IO_INCOMPLETE)
                    {
                        // An error occurred
                        ::ClearCommError(hPortId, &dwCommErrors, &ComStat);
                        break;
                    }

                    // Not finished, wait for it
                }
            }
            else // An error occurred
            {
                ::ClearCommError(hPortId, &dwCommErrors, &ComStat);
            }
        }
    }

    return (int)dwBytesRead;
}

// Write a block of data to the port
int CSerialPort::WriteCommBlock(LPCSTR lpBytes, int nBytesToWrite)
{
    COMSTAT ComStat;
    DWORD   dwBytesWritten = 0;

    // Return zero if handle is invalid
    if (hPortId == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    if (!WriteFile(hPortId, lpBytes, nBytesToWrite, &dwBytesWritten, &olWrite))
    {
        // Did it fail because I/O is still pending?
        dwLastError = ::GetLastError();
        if (dwLastError == ERROR_IO_PENDING)
        {
            // Wait for write to complete or a timeout.
            // NOTE: You can set the last parameter to FALSE for no
            //       waiting, but it tends to consume a lot of CPU
            //       cycles which isn't very friendly.
            while (!GetOverlappedResult(hPortId, &olWrite, &dwBytesWritten,
                                        TRUE))
            {
                dwLastError = ::GetLastError();
                if (dwLastError != ERROR_IO_INCOMPLETE)
                {
                    // An error occurred
                    ::ClearCommError(hPortId, &dwCommErrors, &ComStat);
                    break;
                }

                // Not finished, wait for it
            }
        }
        else // An error occurred
        {
            ::ClearCommError(hPortId, &dwCommErrors, &ComStat);
        }
    }

    return (int)dwBytesWritten;
}

// Setup a wait event for the port.  CheckForCommEvent() can be used to
// see if any events have occurred yet.
//
// NOTE: MSKB Article ID: Q137862 - Unless it has been fixed, a bug in
//       Win95 prevents EV_RING from being seen.
BOOL CSerialPort::WaitCommEvent(void)
{
    dwLastError = ERROR_SUCCESS;
    dwEventMask = 0;

    BOOL bRetVal = ::WaitCommEvent(hPortId, &dwEventMask, &olWait);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
        if (dwLastError == ERROR_IO_PENDING)
        {
            bRetVal = TRUE;
        }
    }

    return bRetVal;
}

// Check to see if any comm events are available.  If so, return what
// has occurred.
DWORD CSerialPort::CheckForCommEvent(BOOL bWait)
{
    DWORD   dwBytesRead;
    COMSTAT ComStat;

    // If we don't want to wait here, return immediately.
    if (!GetOverlappedResult(hPortId, &olWait, &dwBytesRead, bWait))
    {
        if (::GetLastError() != ERROR_IO_INCOMPLETE)
        {
            // An error occurred
            ::ClearCommError(hPortId, &dwCommErrors, &ComStat);
            WaitCommEvent();
        }

        // Else not finished, so nothing is pending

        return 0;
    }

    return dwEventMask;
}

// =========================================================================
// Win32 API wrapper functions
// =========================================================================
BOOL CSerialPort::ClearCommBreak(void)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::ClearCommBreak(hPortId);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::ClearCommError(LPCOMSTAT lpStat, LPDWORD lpdwErrors)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::ClearCommError(hPortId, &dwCommErrors, lpStat);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }
    else if (lpdwErrors)
    {
        *lpdwErrors = dwCommErrors;
    }

    return bRetVal;
}

BOOL CSerialPort::EscapeCommFunction(DWORD dwFunc)
{
    DCB  dcb;
    BOOL bRetVal;

    dwLastError = ERROR_SUCCESS;

    dwLastError   = ERROR_SUCCESS;
    dcb.DCBlength = sizeof(DCB);
    ::GetCommState(hPortId, &dcb);

    // The EscapeComm() function should not be used to adjust line
    // settings if dcb.fDtrControl is set to DTR_CONTROL_HANDSHAKE
    // or dcb.fRtsControl is set to RTS_CONTROL_HANDSHAKE or
    // RTS_CONTROL_TOGGLE (that's what is says in the online help
    // for the DCB structure).
    if ((dcb.fDtrControl & DTR_CONTROL_HANDSHAKE) &&
        (dwFunc & (CLRDTR | SETDTR)))
    {
        return FALSE;
    }

    if ((dcb.fRtsControl & (RTS_CONTROL_HANDSHAKE | RTS_CONTROL_TOGGLE)) &&
        (dwFunc & (CLRRTS | SETRTS)))
    {
        return FALSE;
    }

    bRetVal = ::EscapeCommFunction(hPortId, dwFunc);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::GetCommConfig(LPCOMMCONFIG lpCC, LPDWORD lpdwSize)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::GetCommConfig(hPortId, lpCC, lpdwSize);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::GetCommMask(LPDWORD lpEvtMask)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::GetCommMask(hPortId, lpEvtMask);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::GetCommModemStatus(LPDWORD lpModemStat)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::GetCommModemStatus(hPortId, lpModemStat);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::GetCommProperties(LPCOMMPROP lpCommProp)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::GetCommProperties(hPortId, lpCommProp);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::PurgeComm(DWORD fdwAction)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::PurgeComm(hPortId, fdwAction);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::SetCommBreak(void)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::SetCommBreak(hPortId);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::SetCommConfig(LPCOMMCONFIG lpCC, DWORD dwSize)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::SetCommConfig(hPortId, lpCC, dwSize);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

// NOTE: MSKB Article ID: Q137862 - Unless it has been fixed, a bug in
//       Win95 prevents EV_RING from being seen.
BOOL CSerialPort::SetCommMask(DWORD fdwEvtMask)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::SetCommMask(hPortId, fdwEvtMask);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::TransmitCommChar(char chTransmit)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::TransmitCommChar(hPortId, chTransmit);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

// DeviceIoControl() call to enable/disable insertion of line status
// and modem status values in the normal data stream
//
// NOTE: The online help says that IOCTL_SERIAL_LSRMST_INSERT is NOT
//       supported under Windows 95.
//
BOOL CSerialPort::LSRMST_INSERT(BYTE chEscapeChar)
{
    BOOL  bRetVal;
    DWORD dwBytesReturned;

    dwLastError = ERROR_SUCCESS;

    bRetVal = DeviceIoControl(hPortId, IOCTL_SERIAL_LSRMST_INSERT,
                              &chEscapeChar, 1, NULL, 0, &dwBytesReturned, NULL);

    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

// =========================================================================
// WIn32 API wrappers for those who prefer the direct approach to port
// configuration.
// =========================================================================
BOOL CSerialPort::GetCommState(LPDCB lpDCB)
{
    dwLastError = ERROR_SUCCESS;

    // Set structure size in case the user forgot
    lpDCB->DCBlength = sizeof(DCB);

    BOOL bRetVal = ::GetCommState(hPortId, lpDCB);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::GetCommTimeouts(LPCOMMTIMEOUTS lpCommTimeouts)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::GetCommTimeouts(hPortId, lpCommTimeouts);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

// You can use this function to alter settings not normally handled by
// the class's built-in configuration functions.
BOOL CSerialPort::SetCommState(LPDCB lpDCB)
{
    dwLastError = ERROR_SUCCESS;

    // Set structure size in case the user forgot
    lpDCB->DCBlength = sizeof(DCB);

    BOOL bRetVal = ::SetCommState(hPortId, lpDCB);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::SetCommTimeouts(LPCOMMTIMEOUTS lpCommTimeouts)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::SetCommTimeouts(hPortId, lpCommTimeouts);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

BOOL CSerialPort::SetupComm(DWORD cbInQueue, DWORD cbOutQueue)
{
    dwLastError = ERROR_SUCCESS;

    BOOL bRetVal = ::SetupComm(hPortId, cbInQueue, cbOutQueue);
    if (!bRetVal)
    {
        dwLastError = ::GetLastError();
    }

    return bRetVal;
}

//  Get Com port name and return number of com port in the system
int CSerialPort::GetAllComPort(std::vector<std::string> &vecComPortName)
{
    int i = 0; // initial index of CStringArray

    HKEY hKey;

    std::string strSubKey;

    long lKeyExRes; // Holds the result of Serial comm RegOpenKeyEx function

    DWORD dwType;   // Key value type

//  open HKEY_LOCAL_MACHINE\HARDWARE\\DEVICEMAP\\SERIALCOMM registry key
    strSubKey = _T("HARDWARE\\DEVICEMAP\\SERIALCOMM");
    lKeyExRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strSubKey.c_str(), 0, KEY_QUERY_VALUE, &hKey);

    if (lKeyExRes != ERROR_SUCCESS) // ; // if open key success
    {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            ::GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
            );
        // Process any inserts in lpMsgBuf.
        // ...
        // Display the string.
        TRACE("%s\n", (LPCTSTR)lpMsgBuf);
        // Free the buffer.
        LocalFree(lpMsgBuf);
        return 0;
    }

    DWORD dwIndex         = 0;
    DWORD dwDataSize      = 128; // address for size of data buffer;
    DWORD dwValueNameSize = 128; // address for size of value buffer

    LONG lValueRes = 0;          // Holds the result of the value finds

    char cValueName[255];        // address of buffer for value string

    BYTE byData[255];            // address of buffer for value data

    do
    {
        dwValueNameSize = 128;   // address for size of value buffer
        dwDataSize      = 128;   // Fix RegEnumValue() return Error=14 ERROR_OUTOFMEMORY. Need to initialize to correct value

        lValueRes = RegEnumValue(
            hKey,                // handle to key to query
            dwIndex++,           // index of value to query
            cValueName,          // address of buffer for value string
            &dwValueNameSize,    // address for size of value buffer
            NULL,                // reserved
            &dwType,             // address of buffer for type code
            byData,              // address of buffer for value data
            &dwDataSize          // address for size of data buffer
            );

        if (lValueRes == ERROR_SUCCESS)
        {
//          TRACE("%s = %s\n",cValueName, byData);
            // no case compare first 3 bytes
            if (0 == _mbsnbicmp((const unsigned char *)"COM", byData, 3))
            {
                vecComPortName.push_back(LPCSTR(byData)); // store com port value
            }
        }
//      if error print out the error desciption on the vc++ output window
        else if (lValueRes != ERROR_NO_MORE_ITEMS /* 259 */)
        {
            LPVOID lpMsgBuf;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                ::GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL
                );
            // Process any inserts in lpMsgBuf.
            // ...
            // Display the string.
            TRACE("%s\n", (LPCTSTR)lpMsgBuf);
            // Free the buffer.
            LocalFree(lpMsgBuf);
        }
    } while (lValueRes == ERROR_SUCCESS); // Loop unitl no more Value

    // sort ascending order. COM1, COM2,... so for
    // sort using postfix number to resolve sort COM1, COM2, COM10, COM11 wrongly
    std::sort(vecComPortName.begin(), vecComPortName.end(), 
                        [&](std::string a, std::string b) {  
                                return atoi(a.substr(3).c_str()) < atoi(b.substr(3).c_str()); 
                        });
    //
    return (int) vecComPortName.size();
}

/*
   //   Get Com port name and return number of com port in the system
   int CSerialPort::GetAllComPort(CStringArray &csaComPortName)
   {
   //   num of serial com subkey in HKEY_LOCAL_MACHINE\HARDWARE\\DEVICEMAP\\SERIALCOMM
    int iSerialCom = 0;
    int i = 0; // initial index of CStringArray

    long lRes; // Holds the result of the keys finds
    long lKeyExRes; // Holds the result of Serial comm RegOpenKeyEx function

    HKEY hSerialPortKey; // handle of Serial Port key

    DWORD dwType; // Key value type
    DWORD dwBytes = 128; // sub key max number of bytes

    unsigned char szSubKeyBuffer[256]; // sub key char buffer

    CString csSubKey, csVariableName;

   //   open HKEY_LOCAL_MACHINE\HARDWARE\\DEVICEMAP\\SERIALCOMM registry key
    csSubKey.Format("HARDWARE\\DEVICEMAP\\SERIALCOMM");
    lKeyExRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, csSubKey, 0, KEY_READ, &hSerialPortKey);
    if (lKeyExRes == ERROR_SUCCESS)//; // if open key success
    {
        do
        {
   //           get sub key data
   //           TRACE("SubKeyName = %s\n",szBuffer);
            csVariableName.Format("\\Device\\Serial%d", iSerialCom++);
            lRes = RegQueryValueEx(hSerialPortKey, csVariableName, NULL, &dwType, szSubKeyBuffer,
                    &dwBytes);
   //           if open sub key successful
            if (ERROR_SUCCESS == lRes)
            {
   //               Query sub key value
   //               TRACE("Key value = %s\n",szSubKeyBuffer);
                csaComPortName.SetAtGrow(i++,szSubKeyBuffer); // store sub key value
            }
        }while (lRes == ERROR_SUCCESS); // Loop unitl no more sub key
        RegCloseKey(hSerialPortKey);
    }
    else
    {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            ::GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
        );
        // Process any inserts in lpMsgBuf.
        // ...
        // Display the string.
        TRACE("%s\n",(LPCTSTR)lpMsgBuf);
        // Free the buffer.
        LocalFree( lpMsgBuf );
    }
   //   int iArraySize = csaComPortName.GetSize();
   //   for (i = 0; i < iArraySize; i++)
   //   {
   //       TRACE("Com port name = %s\n", csaComPortName.GetAt(i));
   //   }
    return i; // return number of com port
   }

   //   Get Com port name and return number of com port in the system
   int CSerialPort::GetAllComPort(CStringArray &csaComPortName)
   {
   //   num of subkey in HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\MultifunctionAdapter
    int iMultifunctionAdapter = 0;
    int i = 0; // initial index of CStringArray

    long lRes; // Holds the result of the keys finds
    long lKeyExRes; // Holds the result of MultifunctionAdapter RegOpenKeyEx function
   //   Holds the result of serial controller RegOpenKeyEx function
    long lSerialControllerKeyExRes;

    HKEY hKey; // handle of open key
    HKEY hMultifunctionAdapterKey; // handle of Multifunction Adapter key
    HKEY hSubKey; // handle of sub key

    DWORD dwSubKeyIndex = 0; // sub key initial index
    DWORD dwType; // Key value type
    DWORD dwBytes = 128; // sub key max number of bytes

    unsigned char szSubKeyBuffer[256]; // sub key char buffer

    CString csSubKey;

    do
    {
   //       open HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\MultifunctionAdapter\d registry key
        csSubKey.Format("HARDWARE\\DESCRIPTION\\System\\MultifunctionAdapter\\%d", iMultifunctionAdapter++);
        lKeyExRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, csSubKey, 0, KEY_READ, &hMultifunctionAdapterKey);
        if (lKeyExRes == ERROR_SUCCESS)//; // if open key success
        {
   //           lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
   //           "HARDWARE\\DESCRIPTION\\System\\MultifunctionAdapter\\5\\SerialController", 0,
   //           KEY_READ, &hKey);
            csSubKey += "\\SerialController";
            lSerialControllerKeyExRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, csSubKey, 0, KEY_READ, &hKey);
            if (lSerialControllerKeyExRes == ERROR_SUCCESS) // if open key successful
            {
                do
                {
                    DWORD dwcMaxSubKeyName = 255; // max sub key array size
                    char szBuffer[256]; // key char buffer
        //          get sub key data
                    lRes = RegEnumKeyEx(hKey, dwSubKeyIndex++, szBuffer, &dwcMaxSubKeyName, NULL, NULL,
                        NULL, NULL);
                    if (lRes == ERROR_SUCCESS) // if get sub key successful
                    {
        //              TRACE("SubKeyName = %s\n",szBuffer);
        //              if open sub key successful
                        if (ERROR_SUCCESS == RegOpenKeyEx(hKey, szBuffer, 0, KEY_READ, &hSubKey))
                        {
        //                  Query sub key value
                            RegQueryValueEx(hSubKey, "Identifier", NULL, &dwType, szSubKeyBuffer,
                                &dwBytes);
        //                  TRACE("%s = %s\n",szBuffer, szSubKeyBuffer);
                            RegCloseKey(hSubKey); // close sub key
                            csaComPortName.SetAtGrow(i++,szSubKeyBuffer); // store sub key value
                        }
                    }
                }while (lRes == ERROR_SUCCESS); // Loop unitl no more sub key
                RegCloseKey(hKey); // close key
            }
            RegCloseKey(hMultifunctionAdapterKey);
        }
    }while (lKeyExRes == ERROR_SUCCESS); // Loop unitl no more sub key
   //   int iArraySize = csaComPortName.GetSize();
   //   for (i = 0; i < iArraySize; i++)
   //   {
   //       TRACE("Com port name = %s\n", csaComPortName.GetAt(i));
   //   }
    return i; // return number of com port
   }*/
