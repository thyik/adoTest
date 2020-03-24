// MaintenanceOutputRS.cpp : implementation file
//

#include "stdafx.h"
#include "MaintenanceOutputRS.h"

//#include "HardwareIni.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMaintenanceOutputRS

CMaintenanceOutputRS::CMaintenanceOutputRS()
{
    // {{AFX_FIELD_INIT(CMaintenanceOutputRS)
    m_ID           = 0;
    m_Module_Name  = _T("");
    m_Output       = _T("");
    m_Board_Type   = 0;
    m_Logic        = 0;
    m_Point_No     = 0;
    m_Slave_ID     = 0;
    m_Ptr_No       = 0;
    m_Enable_Check = FALSE;
    m_CardNo       = 0;
    //m_nFields      = 10;
    // }}AFX_FIELD_INIT
    //m_nDefaultType = dbOpenDynaset;

    m_iNoOfModule = 0;

}

CMaintenanceOutputRS::~CMaintenanceOutputRS()
{
    Close();
}

CString CMaintenanceOutputRS::GetDefaultDBName()
{
    // FIXME
    //return _T(MACHINE_DB);
    return _T("d:\\machine\\database\\mitmc.mdb");
}

CString CMaintenanceOutputRS::GetDefaultSQL()
{
    return _T("[Output]");
}

//void CMaintenanceOutputRS::DoFieldExchange(CDaoFieldExchange* pFX)
//{
//    // {{AFX_FIELD_MAP(CMaintenanceOutputRS)
//    pFX->SetFieldType(CDaoFieldExchange::outputColumn);
//    DFX_Long(pFX, _T("[ID]"), m_ID);
//    DFX_Text(pFX, _T("[Module Name]"), m_Module_Name);
//    DFX_Text(pFX, _T("[Output]"), m_Output);
//    DFX_Long(pFX, _T("[Board Type]"), m_Board_Type);
//    DFX_Long(pFX, _T("[Logic]"), m_Logic);
//    DFX_Long(pFX, _T("[Point No]"), m_Point_No);
//    DFX_Long(pFX, _T("[Slave ID]"), m_Slave_ID);
//    DFX_Long(pFX, _T("[Ptr No]"), m_Ptr_No);
//    DFX_Bool(pFX, _T("[Enable Check]"), m_Enable_Check);
//    DFX_Long(pFX, _T("[CardNo]"), m_CardNo);
//    // }}AFX_FIELD_MAP
//}
void CMaintenanceOutputRS::GetAllFieldsValue()
{
    m_rs.GetFieldValue(_T("ID"), m_ID);
    m_rs.GetFieldValue(_T("Module Name"), m_Module_Name);
    m_rs.GetFieldValue(_T("Output"), m_Output);
    m_rs.GetFieldValue(_T("Board Type"), m_Board_Type);
    m_rs.GetFieldValue(_T("Logic"), m_Logic);
    m_rs.GetFieldValue(_T("Point No"), m_Point_No);
    m_rs.GetFieldValue(_T("Slave ID"), m_Slave_ID);
    m_rs.GetFieldValue(_T("Ptr No"), m_Ptr_No);
    m_rs.GetFieldValue(_T("Enable Check"), m_Enable_Check);
    m_rs.GetFieldValue(_T("CardNo"), m_CardNo);
}

bool CMaintenanceOutputRS::Open(std::string sqlQuery)
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

// Function to init output module to maintenance page
void CMaintenanceOutputRS::InitOutputModule(stModule *module)
{
    std::string strSql = _T("SELECT * FROM [Output] ORDER BY [Module Name],[ID]");

    Open(strSql);                 // Creates a new dynaset recordset

    if (m_rs.IsEOF())                                // check if recordset contains records
    {
        //CMessageBox Box(CMultiLanguage::GetMessageText(CMultiLanguage::NO_RECORDS_FOUND_IN_OUTPUT_TABLE));
        //Box.DoModal();                          // call modal dialog box to prompt the above message
        Close();                                // close recordset
        return;
    }

    m_rs.MoveFirst();                                // move to first record to set the orignal of the db pointer
    m_rs.MoveLast();                                 // move to last record
    int iRecordCount = m_rs.GetRecordCount();        // get record count after move to last reord
    m_rs.MoveFirst();                                // move to first record
    // ado2
    GetAllFieldsValue();

    initNoOfOutput(module);                     // initialised all module no of Output to 0
    int iModule = GetModuleIndex(module);
    if (iModule == -1)
    {
        CString csMessage = m_Module_Name + " module not found in Output Table";

        //CMessageBox Box(csMessage);
        //Box.DoModal(); // call modal dialog box to prompt the above message
        Close();       // close recordset
        return;
    }

    int iOutputNo = 0;
    m_stMaintOpModule[iModule].m_csModuleName        = m_Module_Name;                                                       // store module name
    module[iModule].Output[iOutputNo].m_szModuleName = m_stMaintOpModule[iModule].m_csModuleName.operator LPCTSTR();        // set Module name

    m_stMaintOpModule[iModule].m_csIoName[iOutputNo] = m_Output;                                                            // store output name
    module[iModule].Output[iOutputNo].m_szName       = m_stMaintOpModule[iModule].m_csIoName[iOutputNo].operator LPCTSTR(); // set output name

    module[iModule].Output[iOutputNo].m_nBoardType   = m_Board_Type;
    module[iModule].Output[iOutputNo].m_bLogic       = m_Logic;
    module[iModule].Output[iOutputNo].m_nPointNo     = m_Point_No;
    module[iModule].Output[iOutputNo].m_nPtrNo       = m_Ptr_No;
    module[iModule].Output[iOutputNo].m_bEnableCheck = m_Enable_Check;
    module[iModule].Output[iOutputNo].m_nCardNo      = m_CardNo;
    module[iModule].Output[iOutputNo++].m_nSlaveID   = m_Slave_ID;

    for (int i = 1; i < iRecordCount; i++)
    {
        m_rs.MoveNext();        // move to next record
        // ado2
        GetAllFieldsValue();

        if (m_stMaintOpModule[iModule].m_csModuleName == m_Module_Name)                                                             // same module
        {
            module[iModule].Output[iOutputNo].m_szModuleName = m_stMaintOpModule[iModule].m_csModuleName.operator LPCTSTR();        // set Module name

            m_stMaintOpModule[iModule].m_csIoName[iOutputNo] = m_Output;                                                            // store output name
            module[iModule].Output[iOutputNo].m_szName       = m_stMaintOpModule[iModule].m_csIoName[iOutputNo].operator LPCTSTR(); // set output name

            module[iModule].Output[iOutputNo].m_nBoardType   = m_Board_Type;
            module[iModule].Output[iOutputNo].m_bLogic       = m_Logic;
            module[iModule].Output[iOutputNo].m_nPointNo     = m_Point_No;
            module[iModule].Output[iOutputNo].m_nPtrNo       = m_Ptr_No;
            module[iModule].Output[iOutputNo].m_bEnableCheck = m_Enable_Check;
            module[iModule].Output[iOutputNo].m_nCardNo      = m_CardNo;
            module[iModule].Output[iOutputNo++].m_nSlaveID   = m_Slave_ID;
        }
        else // different module
        {
            module[iModule].nNoOfOutput = iOutputNo;
            iOutputNo                   = 0;
            iModule                     = GetModuleIndex(module);

            m_stMaintOpModule[iModule].m_csModuleName        = m_Module_Name;                                                       // store module name
            module[iModule].Output[iOutputNo].m_szModuleName = m_stMaintOpModule[iModule].m_csModuleName.operator LPCTSTR();        // set Module name

            m_stMaintOpModule[iModule].m_csIoName[iOutputNo] = m_Output;                                                            // store output name
            module[iModule].Output[iOutputNo].m_szName       = m_stMaintOpModule[iModule].m_csIoName[iOutputNo].operator LPCTSTR(); // set output name
            module[iModule].Output[iOutputNo].m_nBoardType   = m_Board_Type;
            module[iModule].Output[iOutputNo].m_bLogic       = m_Logic;
            module[iModule].Output[iOutputNo].m_nPointNo     = m_Point_No;
            module[iModule].Output[iOutputNo].m_nPtrNo       = m_Ptr_No;
            module[iModule].Output[iOutputNo].m_bEnableCheck = m_Enable_Check;
            module[iModule].Output[iOutputNo].m_nCardNo      = m_CardNo;
            module[iModule].Output[iOutputNo++].m_nSlaveID   = m_Slave_ID;
        }
    }

    module[iModule].nNoOfOutput = iOutputNo;
    Close(); // close recordset
}

// initialised all module no of input to 0
void CMaintenanceOutputRS::initNoOfOutput(stModule *module)
{
    for (int i = 0; i < MAX_MODULE; i++)
    {
        module[i].nNoOfOutput = 0;
    }
}

// return the module index
int CMaintenanceOutputRS::GetModuleIndex(stModule *module)
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

// return on of modules in output table
int CMaintenanceOutputRS::GetNoOfModule()
{
    m_iNoOfModule = 0;

    std::string strSql = _T("SELECT [Module Name] FROM [Output] ORDER BY [Module Name]"); // select all fields from Output table // ORDER BY [Module Name] ASC");

    Open(strSql);                                                // Creates a new dynaset recordset

    if (m_rs.IsEOF())                                                               // check if recordset contains records
    {
        //CMessageBox Box(CMultiLanguage::GetMessageText(CMultiLanguage::NO_RECORDS_FOUND_IN_OUTPUT_TABLE));
        //Box.DoModal();                                                         // call modal dialog box to prompt the above message
        Close();                                                               // close recordset
        return 0;                                                              // return no modules
    }

    m_rs.MoveFirst();                                                               // move to first record to set the orignal of the db pointer
    m_rs.MoveLast();                                                                // move to last record
    int iRecordCount = m_rs.GetRecordCount();                                       // get record count after move to last reord
    m_rs.MoveFirst();                                                               // move to first record
    // ado2
    GetAllFieldsValue();

    m_stMaintOpModule[m_iNoOfModule].m_csModuleName = m_Module_Name;           // store module name

    for (int i = 1; i < iRecordCount; i++)
    {
        m_rs.MoveNext();                                                            // move to next record
        // ado2
        GetAllFieldsValue();
        //
        if (m_stMaintOpModule[m_iNoOfModule].m_csModuleName != m_Module_Name)  // same module
        {
            m_iNoOfModule++;
            m_stMaintOpModule[m_iNoOfModule].m_csModuleName = m_Module_Name;   // store module name
        }
    }

    Close();                                                                   // close recordset
    return m_iNoOfModule + 1;
}

// return module name in output table
stMaintIoModule* CMaintenanceOutputRS::GetAllModuleName()
{
    return m_stMaintOpModule;
}

// serach output no then return output name
CString CMaintenanceOutputRS::GetOutputName(stModule *module, int iPointNo, int &iModule, int &iOutputNo)
{
    for (int i = 0; i < MAX_MODULE; i++)
    {
        if (module[i].csName.IsEmpty())
        {
            break;
        }

        for (int j = 0; j < module[i].nNoOfOutput; j++)
        {
            // corrected the retrieving of output point name when using
            // MotionNet HSL
            if (module[i].Output[j].m_nBoardType != PCI_7851_ADLINK
                && module[i].Output[j].m_nBoardType != PCI_7856_HSL_ADLINK)
            {
                if (module[i].Output[j].m_nPointNo == (iPointNo -
                                                       ((module[i].Output[j].m_nBoardType * MAX_CARD_FOR_EACH_TYPE * MAX_POINTS_FOR_EACH_CARD) +
                                                        (module[i].Output[j].m_nCardNo * MAX_POINTS_FOR_EACH_CARD))))
                {
                    iModule   = i;
                    iOutputNo = j;
                    return m_stMaintOpModule[i].m_csIoName[j];
                }
            }
            else
            {
                if (module[i].Output[j].m_nPointNo == (iPointNo -
                                                       ((module[i].Output[j].m_nBoardType * MAX_SLAVE_ID * MAX_POINTS_FOR_EACH_SLAVE_ID) +
                                                        (module[i].Output[j].m_nSlaveID * MAX_POINTS_FOR_EACH_SLAVE_ID))))
                {
                    iModule   = i;
                    iOutputNo = j;
                    return m_stMaintOpModule[i].m_csIoName[j];
                }
            }
        }
    }

    CString csPointNo;
    csPointNo.Format("%d", iPointNo);
    CString csMsg = "Output Point " + csPointNo
                    + " Not Found";

    //CMessageBox box(csMsg);
    //box.DoModal();
    return "";
}

// Close Recordset and close database
void CMaintenanceOutputRS::Close()
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
// CMaintenanceOutputRS diagnostics

#ifdef _DEBUG
void CMaintenanceOutputRS::AssertValid() const
{
    //CDaoRecordset::AssertValid();
}

void CMaintenanceOutputRS::Dump(CDumpContext& dc) const
{
    //CDaoRecordset::Dump(dc);
}

#endif // _DEBUG
