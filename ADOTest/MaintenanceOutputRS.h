#if !defined(AFX_MAINTENANCEOUTPUTRS_H__1A7EA31C_6702_4CE7_B658_4BF7BB56FAA0__INCLUDED_)
#define AFX_MAINTENANCEOUTPUTRS_H__1A7EA31C_6702_4CE7_B658_4BF7BB56FAA0__INCLUDED_

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

// MaintenanceOutputRS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMaintenanceOutputRS DAO recordset

class /*DLLCLASS*/ CMaintenanceOutputRS
{
public:
    CMaintenanceOutputRS();
    virtual ~CMaintenanceOutputRS();
// Field/Param Data
// {{AFX_FIELD(CMaintenanceOutputRS, CDaoRecordset)
    long m_ID;
    CString m_Module_Name;
    CString m_Output;
    long    m_Board_Type;
    long    m_Logic;
    long    m_Point_No;
    long    m_Slave_ID;
    long    m_Ptr_No;
    BOOL    m_Enable_Check;
    long    m_CardNo;
// }}AFX_FIELD

// Overrides
// ClassWizard generated virtual function overrides
// {{AFX_VIRTUAL(CMaintenanceOutputRS)
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

    void InitOutputModule(stModule *module); // Function to init output module to maintenance page
    int  GetNoOfModule();                    // return on of modules in output table
//  return module name in output table
    stMaintIoModule* GetAllModuleName();
//  serach output no then return output name
    CString GetOutputName(stModule *module, int iPointNo, int &iModule, int &iOutputNo);

private:
    CADODatabase  m_db;
    CADORecordset m_rs;

    int m_iNoOfModule; // store no of modules in input Table

    stMaintIoModule m_stMaintOpModule[MAX_MODULE];
    CString         m_csPtNoSeqOutputName[MAX_MODULE];

    void GetAllFieldsValue();
    bool Open(std::string sqlQuery);
    int  GetModuleIndex(stModule *module); // return the module index
    void initNoOfOutput(stModule *module); // initialised all module no of output to 0
};

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINTENANCEOUTPUTRS_H__1A7EA31C_6702_4CE7_B658_4BF7BB56FAA0__INCLUDED_)
