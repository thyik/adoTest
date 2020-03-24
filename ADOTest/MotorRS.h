// Start =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// General information section.
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// $Author:yongkiang$ User who last changed the file
// $Date:1/2/2013 12:16:38 PM$ Date and time of last check in
// $Revision:1.0$ Visual SourceSafe version number
// $Workfile::   MotorRS.h                                             $ Filename
// End =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#if !defined(AFX_MOTORRS_H__451481B3_921B_11D6_A80A_005004C029CA__INCLUDED_)
#define AFX_MOTORRS_H__451481B3_921B_11D6_A80A_005004C029CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef  BUILD_MCCTRDLL
#define     DLLCLASS    __declspec(dllexport)
#else
#define     DLLCLASS    __declspec(dllimport)
#endif

//#include <afxdao.h>
#include "Structure.h"
//#include "MyStringArray.h"

#include "ado2.h"

#include <vector>
// MotorRS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMotorRS DAO recordset

class /*DLLCLASS*/ CMotorRS
{
public:
    CMotorRS();
    virtual ~CMotorRS();

// Field/Param Data
// {{AFX_FIELD(CMotorRS, CDaoRecordset)
    long    m_ID;
    CString m_Module_Name;
    CString m_Motor_Name;
    CString m_Position_Name;
    BOOL    m_Teach_Enable;
    BOOL    m_Save_All_Pkg;
// }}AFX_FIELD

// Overrides
// ClassWizard generated virtual function overrides
// {{AFX_VIRTUAL(CMotorRS)
public:
    virtual CString GetDefaultDBName();                      // Default database name
    virtual CString GetDefaultSQL();                         // Default SQL for Recordset
//    virtual void    DoFieldExchange(CDaoFieldExchange* pFX); // RFX support
// }}AFX_VIRTUAL

//  Close Recordset and close database
    virtual void Close();

// Implementation
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    void InitMotorModule(stModule *module); // Function to init motor module to maintenance page
    int  GetNoOfModule();                   // return number of module in motor table
    int  GetNoOfMtr();                      // return number of motor

    CString * GetAllModuleName();           // return module name in motor table
    std::vector<CString> GetAllMtrName();              // return motor name in motor table

//  get motor and pos name
    bool GetMtrAndPosName(stModule *module, int iMtrNo, int iMtrPos, CString &csMtrName, CString &csMtrPos);

//  get motor pos name
    int GetMtrPosNo(stModule *module, CString csModuleName, int iMtrNo, CString csMtrPos,
                    CString csSubPosName);

//  get Save all pkg
    BOOL GetSaveAllPkg(CString csPosName);
    int  InitModule(stModule *module);
    //  set the motor position name from Motor and MotorSubField table
    void SetMtrPosName(stModule *module);

private:
    CADODatabase  m_db;
    CADORecordset m_rs;

    int m_iNoOfModule;                                 // store no of modules in Motor Table

    CString        m_csModuleName[MAX_MODULE];         // store module name
    std::vector<CString>        m_vecMtrName;             // store Motor name
    std::vector<CString>        m_vecMtrPos[MAX_MODULE][MAX_MOTOR]; // store Motor Position Name

    CString GetConnectionString(CString csDbName);
    bool Open(std::string sqlQuery);
    void GetAllFieldsValue();

    int  GetModuleIndex(stModule *module);             // return the module index
    void initNoOfMtr(stModule *module);                // initialised no of motor to 0
};

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOTORRS_H__451481B3_921B_11D6_A80A_005004C029CA__INCLUDED_)
