#if !defined(AFX_MTRPROFILERS_H__C33C1457_C7CF_4F89_B1FC_C2CC5D0AC3C1__INCLUDED_)
#define AFX_MTRPROFILERS_H__C33C1457_C7CF_4F89_B1FC_C2CC5D0AC3C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MtrProfileRS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMtrProfileRS DAO recordset
#include <string>

#include "structure.h"
#include "ado2.h"

class CMtrProfileRS 
{
public:
    CMtrProfileRS();
    virtual ~CMtrProfileRS();

// Field/Param Data
// {{AFX_FIELD(CMtrProfileRS, CDaoRecordset)
    BOOL m__EL;
    long    m__ve_Dir;
    double  m_Acce;
    BOOL    m_ALM;
    long    m_CardType;
    long    m_Clk;
    BOOL    m_CLR;
    double  m_Const;
    double  m_Cruise;
    double  m_Dec;
    BOOL    m__EL2;
    long    m_EPPR;
    BOOL    m_EZ;
    long    m_Home_Mode;
    long    m_ID;
    BOOL    m_Index_Logic;
    BOOL    m_INP;
    CString m_Motor_Name;
    long    m_Motor_No;
    BOOL    m_ORGI;
    long    m_PPR;
    BOOL    m_RDY;
    double  m_SAcce;
    double  m_SDec;
    BOOL    m_SON;
    double  m_StartUp;
    long    m_Type;
    double  m_UPR;
    double  m_HomeLeaveSteps;
    long    m_Home_Dir;
    long    m_EncoderType;
    double  m_Negative_Soft_Limit;
    double  m_Positive_Soft_Limit;
    long    m_CardNo;
// }}AFX_FIELD

// Overrides
// ClassWizard generated virtual function overrides
// {{AFX_VIRTUAL(CMtrProfileRS)
public:
    virtual CString GetDefaultDBName();                      // Default database name
    virtual CString GetDefaultSQL();                         // Default SQL for Recordset
    //virtual void    DoFieldExchange(CDaoFieldExchange* pFX); // RFX support
// }}AFX_VIRTUAL

//  Close Recordset and close database
    virtual void Close();

//  read motor profile table from mitmc.mdb and write to stModule struct
    void InitMtrProfile(stModule *module);

//  get the module and motor index
    void GetModuleAndMtrIndex(stModule *module, int &iModule, int &iMtrNo);

//  assign motor profile values
    void AssignMtrProfileValues(stModule *module, int iMotorPosArrayIndex);

// Implementation
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:
    CADODatabase  m_db;
    CADORecordset m_rs;


    void GetAllFieldsValue();
    bool Open(std::string sqlQuery);

};

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MTRPROFILERS_H__C33C1457_C7CF_4F89_B1FC_C2CC5D0AC3C1__INCLUDED_)
