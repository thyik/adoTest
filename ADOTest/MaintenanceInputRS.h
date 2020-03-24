#if !defined(AFX_MAINTENANCEINPUTRS_H__EA682405_88BC_4E5B_A0C8_7E53CAA7B510__INCLUDED_)
#define AFX_MAINTENANCEINPUTRS_H__EA682405_88BC_4E5B_A0C8_7E53CAA7B510__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef  BUILD_MCCTRDLL
#define     DLLCLASS    __declspec(dllexport)
#else
#define     DLLCLASS    __declspec(dllimport)
#endif

#include "Structure.h"
#include "ado2.h"

#include <string>

// MaintenanceInputRS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMaintenanceInputRS DAO recordset

class /*DLLCLASS*/ CMaintenanceInputRS
{
public:
    CMaintenanceInputRS();
    virtual ~CMaintenanceInputRS();
    //DECLARE_DYNAMIC(CMaintenanceInputRS)

// Field/Param Data
// {{AFX_FIELD(CMaintenanceInputRS, CDaoRecordset)
    long m_ID;
    CString m_Module_Name;
    CString m_Input;
    long    m_Board_Type;
    long    m_Logic;
    long    m_Point_No;
    long    m_Slave_ID;
    long    m_Ptr_No;
    long    m_CardNo;
// }}AFX_FIELD

// Overrides
// ClassWizard generated virtual function overrides
// {{AFX_VIRTUAL(CMaintenanceInputRS)
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

    void             InitInputModule(stModule *module); // Function to init input module to maintenance page
    int              GetNoOfModule();                   // return on of modules in input table
    stMaintIoModule* GetAllModuleName();

private:
    CADODatabase  m_db;
    CADORecordset m_rs;

    int m_iNoOfModule;                     // store no of modules in input Table

    stMaintIoModule m_stMaintIpModule[MAX_MODULE];


    bool Open(std::string sql);
    void GetAllFieldsValue();


    int  GetModuleIndex(stModule *module); // return the module index
    void initNoOfInput(stModule *module);  // initialised all module no of input to 0
};

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINTENANCEINPUTRS_H__EA682405_88BC_4E5B_A0C8_7E53CAA7B510__INCLUDED_)
