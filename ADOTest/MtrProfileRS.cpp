// MtrProfileRS.cpp : implementation file
//

#include "stdafx.h"
#include "MtrProfileRS.h"
//#include "PromptExceptionMessage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMtrProfileRS

CMtrProfileRS::CMtrProfileRS()
{
    // {{AFX_FIELD_INIT(CMtrProfileRS)
    m__EL                 = FALSE;
    m__ve_Dir             = 0;
    m_Acce                = 0.0;
    m_ALM                 = FALSE;
    m_CardType            = 0;
    m_Clk                 = 0;
    m_CLR                 = FALSE;
    m_Const               = 0.0;
    m_Cruise              = 0.0;
    m_Dec                 = 0.0;
    m__EL2                = FALSE;
    m_EPPR                = 0;
    m_EZ                  = FALSE;
    m_Home_Mode           = 0;
    m_ID                  = 0;
    m_Index_Logic         = FALSE;
    m_INP                 = FALSE;
    m_Motor_Name          = _T("");
    m_Motor_No            = 0;
    m_ORGI                = FALSE;
    m_PPR                 = 0;
    m_RDY                 = FALSE;
    m_SAcce               = 0.0;
    m_SDec                = 0.0;
    m_SON                 = FALSE;
    m_StartUp             = 0.0;
    m_Type                = 0;
    m_UPR                 = 0.0;
    m_HomeLeaveSteps      = 0.0;
    m_Home_Dir            = 0;
    m_EncoderType         = 0;
    m_Negative_Soft_Limit = 0.0;
    m_Positive_Soft_Limit = 0.0;
    m_CardNo              = 0;
    //m_nFields             = 34;
    // }}AFX_FIELD_INIT
    //m_nDefaultType = dbOpenDynaset;
}

CMtrProfileRS::~CMtrProfileRS()
{
    Close();
}

CString CMtrProfileRS::GetDefaultDBName()
{
    // FIXME
    //return _T(MACHINE_DB);
    return _T("d:\\machine\\database\\mitmc.mdb");
}

CString CMtrProfileRS::GetDefaultSQL()
{
    return _T("[MotorProfile]");
}

//void CMtrProfileRS::DoFieldExchange(CDaoFieldExchange* pFX)
//{
//    // {{AFX_FIELD_MAP(CMtrProfileRS)
//    pFX->SetFieldType(CDaoFieldExchange::outputColumn);
//    DFX_Bool(pFX, _T("[+EL]"), m__EL);
//    DFX_Long(pFX, _T("[+ve Dir]"), m__ve_Dir);
//    DFX_Double(pFX, _T("[Acce]"), m_Acce);
//    DFX_Bool(pFX, _T("[ALM]"), m_ALM);
//    DFX_Long(pFX, _T("[CardType]"), m_CardType);
//    DFX_Long(pFX, _T("[Clk]"), m_Clk);
//    DFX_Bool(pFX, _T("[CLR]"), m_CLR);
//    DFX_Double(pFX, _T("[Const]"), m_Const);
//    DFX_Double(pFX, _T("[Cruise]"), m_Cruise);
//    DFX_Double(pFX, _T("[Dec]"), m_Dec);
//    DFX_Bool(pFX, _T("[-EL]"), m__EL2);
//    DFX_Long(pFX, _T("[EPPR]"), m_EPPR);
//    DFX_Bool(pFX, _T("[EZ]"), m_EZ);
//    DFX_Long(pFX, _T("[Home Mode]"), m_Home_Mode);
//    DFX_Long(pFX, _T("[ID]"), m_ID);
//    DFX_Bool(pFX, _T("[Index Logic]"), m_Index_Logic);
//    DFX_Bool(pFX, _T("[INP]"), m_INP);
//    DFX_Text(pFX, _T("[Motor Name]"), m_Motor_Name);
//    DFX_Long(pFX, _T("[Motor No]"), m_Motor_No);
//    DFX_Bool(pFX, _T("[ORGI]"), m_ORGI);
//    DFX_Long(pFX, _T("[PPR]"), m_PPR);
//    DFX_Bool(pFX, _T("[RDY]"), m_RDY);
//    DFX_Double(pFX, _T("[SAcce]"), m_SAcce);
//    DFX_Double(pFX, _T("[SDec]"), m_SDec);
//    DFX_Bool(pFX, _T("[SON]"), m_SON);
//    DFX_Double(pFX, _T("[StartUp]"), m_StartUp);
//    DFX_Long(pFX, _T("[Type]"), m_Type);
//    DFX_Double(pFX, _T("[UPR]"), m_UPR);
//    DFX_Double(pFX, _T("[HomeLeaveSteps]"), m_HomeLeaveSteps);
//    DFX_Long(pFX, _T("[Home Dir]"), m_Home_Dir);
//    DFX_Long(pFX, _T("[EncoderType]"), m_EncoderType);
//    DFX_Double(pFX, _T("[Negative Soft Limit]"), m_Negative_Soft_Limit);
//    DFX_Double(pFX, _T("[Positive Soft Limit]"), m_Positive_Soft_Limit);
//    DFX_Long(pFX, _T("[CardNo]"), m_CardNo);
//    // }}AFX_FIELD_MAP
//}

void CMtrProfileRS::GetAllFieldsValue()
{
    m_rs.GetFieldValue(_T("+EL"), m__EL);
    m_rs.GetFieldValue(_T("+ve Dir"), m__ve_Dir);
    m_rs.GetFieldValue(_T("Acce"), m_Acce);
    m_rs.GetFieldValue(_T("ALM"), m_ALM);
    m_rs.GetFieldValue(_T("CardType"), m_CardType);
    m_rs.GetFieldValue(_T("Clk"), m_Clk);
    m_rs.GetFieldValue(_T("CLR"), m_CLR);
    m_rs.GetFieldValue(_T("Const"), m_Const);
    m_rs.GetFieldValue(_T("Cruise"), m_Cruise);
    m_rs.GetFieldValue(_T("Dec"), m_Dec);
    m_rs.GetFieldValue(_T("-EL"), m__EL2);
    m_rs.GetFieldValue(_T("EPPR"), m_EPPR);
    m_rs.GetFieldValue(_T("EZ"), m_EZ);
    m_rs.GetFieldValue(_T("Home Mode"), m_Home_Mode);
    m_rs.GetFieldValue(_T("ID"), m_ID);
    m_rs.GetFieldValue(_T("Index Logic"), m_Index_Logic);
    m_rs.GetFieldValue(_T("INP"), m_INP);
    m_rs.GetFieldValue(_T("Motor Name"), m_Motor_Name);
    m_rs.GetFieldValue(_T("Motor No"), m_Motor_No);
    m_rs.GetFieldValue(_T("ORGI"), m_ORGI);
    m_rs.GetFieldValue(_T("PPR"), m_PPR);
    m_rs.GetFieldValue(_T("RDY"), m_RDY);
    m_rs.GetFieldValue(_T("SAcce"), m_SAcce);
    m_rs.GetFieldValue(_T("SDec"), m_SDec);
    m_rs.GetFieldValue(_T("SON"), m_SON);
    m_rs.GetFieldValue(_T("StartUp"), m_StartUp);
    m_rs.GetFieldValue(_T("Type"), m_Type);
    m_rs.GetFieldValue(_T("UPR"), m_UPR);
    m_rs.GetFieldValue(_T("HomeLeaveSteps"), m_HomeLeaveSteps);
    m_rs.GetFieldValue(_T("Home Dir"), m_Home_Dir);
    m_rs.GetFieldValue(_T("EncoderType"), m_EncoderType);
    m_rs.GetFieldValue(_T("Negative Soft Limit"), m_Negative_Soft_Limit);
    m_rs.GetFieldValue(_T("Positive Soft Limit"), m_Positive_Soft_Limit);
    m_rs.GetFieldValue(_T("CardNo"), m_CardNo);

}

bool CMtrProfileRS::Open(std::string sqlQuery)
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

// assign motor profile values
void CMtrProfileRS::AssignMtrProfileValues(stModule *module, int iMotorPosArrayIndex)
{
    int iModule, iMtrNo;

    GetModuleAndMtrIndex(module, iModule, iMtrNo); // get the module and motor index
    if (-1 == iModule || -1 == iMtrNo)             // Module and motor index not found
    {
        CString     csMessage = m_Motor_Name + " motor not found in motor profile table";
        //CMessageBox Box(csMessage);
        //Box.DoModal();                             // call modal dialog box to prompt the above message
        //Close();                                   // close recordset
        return;
    }

    module[iModule].Motor[iMtrNo]->m_iALM = m_ALM;
    module[iModule].Motor[iMtrNo]->m_iClk = m_Clk;
    module[iModule].Motor[iMtrNo]->m_iCLR = m_CLR;
    // module[iModule].Motor[iMtrNo]->m_iEPPR = m_EPPR;
    module[iModule].Motor[iMtrNo]->m_dEPPR       = m_EPPR;
    module[iModule].Motor[iMtrNo]->m_iHomeMode   = m_Home_Mode;
    module[iModule].Motor[iMtrNo]->m_iHomeDir    = m_Home_Dir;
    module[iModule].Motor[iMtrNo]->m_iIndexLogic = m_Index_Logic;
    module[iModule].Motor[iMtrNo]->m_iINP        = m_INP;
    module[iModule].Motor[iMtrNo]->m_iMtrNo      = m_Motor_No;
    module[iModule].Motor[iMtrNo]->m_iMtrType    = m_Type;
    module[iModule].Motor[iMtrNo]->m_iNegEL      = m__EL2;
    module[iModule].Motor[iMtrNo]->m_iORGI       = m_ORGI;
    module[iModule].Motor[iMtrNo]->m_iPosDir     = m__ve_Dir;
    module[iModule].Motor[iMtrNo]->m_iPosEL      = m__EL;
//  module[iModule].Motor[iMtrNo]->m_iPPR = m_PPR;
    module[iModule].Motor[iMtrNo]->m_dPPR = m_PPR;
    module[iModule].Motor[iMtrNo]->m_iRDY = m_RDY;
    module[iModule].Motor[iMtrNo]->m_iSON = m_SON;
//  module[iModule].Motor[iMtrNo]->m_iUpr = m_UPR;
    module[iModule].Motor[iMtrNo]->m_dUpr      = m_UPR;
    module[iModule].Motor[iMtrNo]->m_iCardType = m_CardType;
    module[iModule].Motor[iMtrNo]->m_iCardNo   = m_CardNo;

    module[iModule].Motor[iMtrNo]->m_dConst          = m_Const;
    module[iModule].Motor[iMtrNo]->m_dCruise         = m_Cruise;
    module[iModule].Motor[iMtrNo]->m_dStartUp        = m_StartUp;
    module[iModule].Motor[iMtrNo]->m_dAcce           = m_Acce;
    module[iModule].Motor[iMtrNo]->m_dDec            = m_Dec;
    module[iModule].Motor[iMtrNo]->m_dSAcce          = m_SAcce;
    module[iModule].Motor[iMtrNo]->m_dSDec           = m_SDec;
    module[iModule].Motor[iMtrNo]->m_dHomeLeaveSteps = m_HomeLeaveSteps;
    module[iModule].Motor[iMtrNo]->m_iEncoderType    = m_EncoderType;
    module[iModule].Motor[iMtrNo]->m_dPosSoftLimit   = m_Positive_Soft_Limit;
    module[iModule].Motor[iMtrNo]->m_dNegSoftLimit   = m_Negative_Soft_Limit;

    module[iModule].Motor[iMtrNo]->m_iMotorPosArrayIndex = iMotorPosArrayIndex;
}

//  read motor profile table from mitmc.mdb and write to stModule struct
void CMtrProfileRS::InitMtrProfile(stModule *module)
{
    try
    {
        std::string strSQL = _T("SELECT * FROM [MotorProfile]"); // select all fields from Motor profile table // ORDER BY [Motor No] ASC");

        Open(strSQL);                   // Creates a new dynaset recordset

        if (m_rs.IsEOF())                                  // check if recordset contains records
        {
            //CMessageBox Box(CMultiLanguage::GetMessageText(CMultiLanguage::NO_RECORD_IN_MOTORPROFILE_TABLE));
            //Box.DoModal();                            // call modal dialog box to prompt the above message
            Close();                                  // close recordset
            return;
        }

        m_rs.MoveFirst();                                  // move to first record to set the orignal of the db pointer
        m_rs.MoveLast();                                   // move to last record
        int iRecordCount = m_rs.GetRecordCount();          // get record count after move to last reord
        m_rs.MoveFirst();                                  // move to first record

        for (int i = 0; i < iRecordCount; i++)
        {
            // ado2
            GetAllFieldsValue();
            //
            AssignMtrProfileValues(module, i);        // assign motor profile values
            m_rs.MoveNext();                               // Move to next record
        }
    }
    //catch (CDaoException* DaoEx)                      // catch CDaoException
    //{
    //    // prompt error
    //    CPromptExceptionMessage::promptDaoException(DaoEx, "MitMc.mdb", GetDefaultSQL());
    //    DaoEx->Delete();                              // must delete to avoid memory leak!
    //}
    catch (CMemoryException* MemEx)                   // catch Memory exception
    {
        // prompt error
        //CPromptExceptionMessage::promptMemoryException(MemEx, "MitMc.mdb");
        MemEx->Delete();                              // must delete to avoid memory leak!
    }
    catch (CException *e)
    {
        e->ReportError();
        e->Delete(); // must delete to avoid memory leak!
    }

    Close();         // close record set
}

// get the module and motor index
void CMtrProfileRS::GetModuleAndMtrIndex(stModule *module, int &iModule, int &iMtrNo)
{
    for (int i = 0; i < MAX_MODULE; i++)
    {
        for (int j = 0; j < module[i].nNoOfMotor; j++)
        {
            if (module[i].Motor[j]->m_csMotorName == m_Motor_Name) // motor name found
            {
                iModule = i;                                       // set module index
                iMtrNo  = j;                                       // set motor no index
                return;                                            // return to calling function once module and motor index found
            }
        }
    }

//  module and motor index not found
    iModule = -1;
    iMtrNo  = -1;
}

// Close Recordset and close database
void CMtrProfileRS::Close()
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
// CMtrProfileRS diagnostics

#ifdef _DEBUG
void CMtrProfileRS::AssertValid() const
{
    //CDaoRecordset::AssertValid();
}

void CMtrProfileRS::Dump(CDumpContext& dc) const
{
    //CDaoRecordset::Dump(dc);
}

#endif // _DEBUG
