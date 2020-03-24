// MaintenanceInputRS.cpp : implementation file
//

#include "stdafx.h"
#include "MaintenanceInputRS.h"
//#include "MultiLanguage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMaintenanceInputRS

//IMPLEMENT_DYNAMIC(CMaintenanceInputRS, CDaoRecordset)

CMaintenanceInputRS::CMaintenanceInputRS()
{
    // {{AFX_FIELD_INIT(CMaintenanceInputRS)
    m_ID          = 0;
    m_Module_Name = _T("");
    m_Input       = _T("");
    m_Board_Type  = 0;
    m_Logic       = 0;
    m_Point_No    = 0;
    m_Slave_ID    = 0;
    m_Ptr_No      = 0;
    m_CardNo      = 0;
//    m_nFields     = 9;
    // }}AFX_FIELD_INIT
//    m_nDefaultType = dbOpenDynaset;

    m_iNoOfModule = 0;
}

CMaintenanceInputRS::~CMaintenanceInputRS()
{
    Close();
}

CString CMaintenanceInputRS::GetDefaultDBName()
{
    // FIXME
    //return _T(MACHINE_DB);
    return _T("d:\\machine\\database\\mitmc.mdb");
}

CString CMaintenanceInputRS::GetDefaultSQL()
{
    return _T("[Input]");
}

bool CMaintenanceInputRS::Open(std::string sqlQuery)
{
    CString strConnectionFormat;
    CString strConnection = _T("");
#ifdef _WIN64
    //strConnection = _T("Provider=Microsoft.ACE.OLEDB.12.0;Persist Security Info=False;"\
    //         "Data Source=D:\\Machine\\Database\\MitPkg.mdb");
    strConnectionFormat = _T("Provider=Microsoft.ACE.OLEDB.12.0;Persist Security Info=False;"\
             "Data Source=");
#else
    // for 32-bit process
    strConnectionFormat = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=";
#endif
    //
    strConnection = strConnectionFormat + GetDefaultDBName();
    //
    m_db.SetConnectionString(strConnection);

    if(m_db.Open())
    {
        if(m_rs.Open(m_db.m_pConnection, sqlQuery.c_str(), CADORecordset::openQuery))
        {
            return true;
        }
    }

    return false;
}

//void CMaintenanceInputRS::DoFieldExchange(CDaoFieldExchange* pFX)
//{
//    // {{AFX_FIELD_MAP(CMaintenanceInputRS)
//    pFX->SetFieldType(CDaoFieldExchange::outputColumn);
//    DFX_Long(pFX, _T("[ID]"), m_ID);
//    DFX_Text(pFX, _T("[Module Name]"), m_Module_Name);
//    DFX_Text(pFX, _T("[Input]"), m_Input);
//    DFX_Long(pFX, _T("[Board Type]"), m_Board_Type);
//    DFX_Long(pFX, _T("[Logic]"), m_Logic);
//    DFX_Long(pFX, _T("[Point No]"), m_Point_No);
//    DFX_Long(pFX, _T("[Slave ID]"), m_Slave_ID);
//    DFX_Long(pFX, _T("[Ptr No]"), m_Ptr_No);
//    DFX_Long(pFX, _T("[CardNo]"), m_CardNo);
//    // }}AFX_FIELD_MAP
//}

void CMaintenanceInputRS::GetAllFieldsValue()
{
    m_rs.GetFieldValue(_T("ID"), m_ID);
    m_rs.GetFieldValue(_T("Module Name"), m_Module_Name);
    m_rs.GetFieldValue(_T("Input"), m_Input);
    m_rs.GetFieldValue(_T("Board Type"), m_Board_Type);
    m_rs.GetFieldValue(_T("Logic"), m_Logic);
    m_rs.GetFieldValue(_T("Point No"), m_Point_No);
    m_rs.GetFieldValue(_T("Slave ID"), m_Slave_ID);
    m_rs.GetFieldValue(_T("Ptr No"), m_Ptr_No);
    m_rs.GetFieldValue(_T("CardNo"), m_CardNo);
}

// Function to init input module to maintenance page
void CMaintenanceInputRS::InitInputModule(stModule *module)
{
    std::string strSQL;

    strSQL = _T("SELECT * FROM [Input] ORDER BY [Module Name],[ID]"); // select all fields from Input table // ORDER BY [Module Name] ASC");

    Open(strSQL);          // Creates a new dynaset recordset
    if (m_rs.IsEOF())                         // check if recordset contains records
    {
        // FIXME
        //CMessageBox Box("No Records Found in Input Table");//CMultiLanguage::GetMessageText(CMultiLanguage::NO_RECORDS_FOUND_IN_INPUT_TABLE));
        //Box.DoModal();                   // call modal dialog box to prompt the above message
        //Close();                         // close recordset
        return;
    }

    m_rs.MoveFirst();                         // move to first record to set the orignal of the db pointer
    m_rs.MoveLast();                          // move to last record
    int iRecordCount = m_rs.GetRecordCount(); // get record count after move to last reord
    m_rs.MoveFirst();                         // move to first record
    // ado2
    GetAllFieldsValue();

    initNoOfInput(module);               // initialised all module no of input to 0
    int iModule = GetModuleIndex(module);
    if (iModule == -1)
    {
        CString csMessage = m_Module_Name + " Not Found in Input Table"; //CMultiLanguage::GetMessageText(CMultiLanguage::MODULE_AT_INPUT_TABLE_NOT_FOUND_AT_MODULE_TABLE);

        // FIXME
        //CMessageBox Box(csMessage);
        //Box.DoModal(); // call modal dialog box to prompt the above message
        //Close();       // close recordset
        return;
    }

    int iInputNo = 0;
    // ado2
    GetAllFieldsValue();
    //
    m_stMaintIpModule[iModule].m_csModuleName      = m_Module_Name;                                                       // store module name
    module[iModule].Input[iInputNo].m_szModuleName = m_stMaintIpModule[iModule].m_csModuleName.operator LPCTSTR();        // set Module name

    m_stMaintIpModule[iModule].m_csIoName[iInputNo] = m_Input;                                                            // store input name
    module[iModule].Input[iInputNo].m_szName        = m_stMaintIpModule[iModule].m_csIoName[iInputNo].operator LPCTSTR(); // set input name

    module[iModule].Input[iInputNo].m_nBoardType = m_Board_Type;
    module[iModule].Input[iInputNo].m_bLogic     = m_Logic;
    module[iModule].Input[iInputNo].m_nPointNo   = m_Point_No;
    module[iModule].Input[iInputNo].m_nPtrNo     = m_Ptr_No;
    module[iModule].Input[iInputNo].m_nCardNo    = m_CardNo;
    module[iModule].Input[iInputNo++].m_nSlaveID = m_Slave_ID;

    for (int i = 1; i < iRecordCount; i++)
    {
        m_rs.MoveNext();                                                                                                               // move to next record
        // ado2
        GetAllFieldsValue();

        if (m_stMaintIpModule[iModule].m_csModuleName == m_Module_Name)                                                           // same module
        {
            module[iModule].Input[iInputNo].m_szModuleName = m_stMaintIpModule[iModule].m_csModuleName.operator LPCTSTR();        // set Module name

            m_stMaintIpModule[iModule].m_csIoName[iInputNo] = m_Input;                                                            // store input name
            module[iModule].Input[iInputNo].m_szName        = m_stMaintIpModule[iModule].m_csIoName[iInputNo].operator LPCTSTR(); // set input name

            module[iModule].Input[iInputNo].m_nBoardType = m_Board_Type;
            module[iModule].Input[iInputNo].m_bLogic     = m_Logic;
            module[iModule].Input[iInputNo].m_nPointNo   = m_Point_No;
            module[iModule].Input[iInputNo].m_nPtrNo     = m_Ptr_No;
            module[iModule].Input[iInputNo].m_nCardNo    = m_CardNo;
            module[iModule].Input[iInputNo++].m_nSlaveID = m_Slave_ID;
        }
        else // different module
        {
            module[iModule].nNoOfInput = iInputNo;
            iInputNo                   = 0;
            iModule                    = GetModuleIndex(module);

            m_stMaintIpModule[iModule].m_csModuleName      = m_Module_Name;                                                       // store module name
            module[iModule].Input[iInputNo].m_szModuleName = m_stMaintIpModule[iModule].m_csModuleName.operator LPCTSTR();        // set Module name

            m_stMaintIpModule[iModule].m_csIoName[iInputNo] = m_Input;                                                            // store input name
            module[iModule].Input[iInputNo].m_szName        = m_stMaintIpModule[iModule].m_csIoName[iInputNo].operator LPCTSTR(); // set input name

            module[iModule].Input[iInputNo].m_nBoardType = m_Board_Type;
            module[iModule].Input[iInputNo].m_bLogic     = m_Logic;
            module[iModule].Input[iInputNo].m_nPointNo   = m_Point_No;
            module[iModule].Input[iInputNo].m_nPtrNo     = m_Ptr_No;
            module[iModule].Input[iInputNo].m_nCardNo    = m_CardNo;
            module[iModule].Input[iInputNo++].m_nSlaveID = m_Slave_ID;
        }
    }

    module[iModule].nNoOfInput = iInputNo;
    Close(); // close recordset
}

// initialised all module no of input to 0
void CMaintenanceInputRS::initNoOfInput(stModule *module)
{
    for (int i = 0; i < MAX_MODULE; i++)
    {
        module[i].nNoOfInput = 0;
    }
}

// return the module index
int CMaintenanceInputRS::GetModuleIndex(stModule *module)
{
    for (int i = 0; i < MAX_MODULE; i++)
    {
        if (module[i].csName == m_Module_Name)
        {
            return i;
        }
    }

    ASSERT(FALSE);
    return 0;
}

// return on of modules in input table
int CMaintenanceInputRS::GetNoOfModule()
{
    m_iNoOfModule = 0;

    std::string strSQL = _T("SELECT [Module Name] FROM [Input] ORDER BY [Module Name]"); // select all fields from Input table // ORDER BY [Module Name] ASC");

    Open(strSQL);                                               // Creates a new dynaset recordset

    if (m_rs.IsEOF())                                                              // check if recordset contains records
    {
        // FIXME
        //CMessageBox Box("No records found in Input Table"); //CMultiLanguage::GetMessageText(CMultiLanguage::NO_RECORDS_FOUND_IN_INPUT_TABLE));
        //Box.DoModal();                                                        // call modal dialog box to prompt the above message
        //Close();                                                              // close recordset
        return 0;                                                             // return no modules
    }

    m_rs.MoveFirst();                                                              // move to first record to set the orignal of the db pointer
    m_rs.MoveLast();                                                               // move to last record
    int iRecordCount = m_rs.GetRecordCount();                                      // get record count after move to last reord
    m_rs.MoveFirst();                                                              // move to first record

    // ado2
    GetAllFieldsValue();
    //
    m_stMaintIpModule[m_iNoOfModule].m_csModuleName = m_Module_Name;          // store module name

    for (int i = 1; i < iRecordCount; i++)
    {
        m_rs.MoveNext();                                                           // move to next record
        // ado2
        GetAllFieldsValue();

        if (m_stMaintIpModule[m_iNoOfModule].m_csModuleName != m_Module_Name) // same module
        {
            m_iNoOfModule++;
            m_stMaintIpModule[m_iNoOfModule].m_csModuleName = m_Module_Name;  // store module name
        }
    }

    Close();                                                                  // close recordset
    return m_iNoOfModule + 1;
}

// return module name in input table
stMaintIoModule* CMaintenanceInputRS::GetAllModuleName()
{
    return m_stMaintIpModule;
}

// Close Recordset and close database
void CMaintenanceInputRS::Close()
{
    if (m_rs.IsOpen())               // if recordset is open
    {
        m_rs.Close(); // close recordset
    }

    if (m_db.IsOpen())  // if database is open
    {
        m_db.Close();   // close database
    }
}

/////////////////////////////////////////////////////////////////////////////
// CMaintenanceInputRS diagnostics

#ifdef _DEBUG
void CMaintenanceInputRS::AssertValid() const
{
    //CADORecordset::AssertValid();
}

void CMaintenanceInputRS::Dump(CDumpContext& dc) const
{
    //CADORecordset::Dump(dc);
}

#endif // _DEBUG
