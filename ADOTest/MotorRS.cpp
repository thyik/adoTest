// Start =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// General information section.
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// $Author:yongkiang$ User who last changed the file
// $Date:1/2/2013 12:16:37 PM$ Date and time of last check in
// $Revision:1.0$ Visual SourceSafe version number
// $Workfile::   MotorRS.cpp                                           $ Filename
// End =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// MotorRS.cpp : implementation file
//

#include "stdafx.h"
#include "MotorRS.h"
//#include "PromptExceptionMessage.h"
//#include "MyStringArray.h"
//#include "DbData.h"
#include "MtrProfileRS.h"
//#include "MultiLanguage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMotorRS


CMotorRS::CMotorRS()
{
    // {{AFX_FIELD_INIT(CMotorRS)
    m_ID            = 0;
    m_Module_Name   = _T("");
    m_Motor_Name    = _T("");
    m_Position_Name = _T("");
    m_Teach_Enable  = FALSE;
    m_Save_All_Pkg  = FALSE;
//    m_nFields       = 6;
    // }}AFX_FIELD_INIT
//    m_nDefaultType = dbOpenDynaset;
    m_iNoOfModule  = 0;
}

CMotorRS::~CMotorRS()
{
    Close();
}

CString CMotorRS::GetDefaultDBName()
{
    // FIXME
    //return _T(MACHINE_DB);
    return _T("d:\\machine\\database\\mitmc.mdb");
}

CString CMotorRS::GetDefaultSQL()
{
    return _T("[Motor]");
}

//void CMotorRS::DoFieldExchange(CDaoFieldExchange* pFX)
//{
//    // {{AFX_FIELD_MAP(CMotorRS)
//    pFX->SetFieldType(CDaoFieldExchange::outputColumn);
//    DFX_Long(pFX, _T("[ID]"), m_ID);
//    DFX_Text(pFX, _T("[Module Name]"), m_Module_Name);
//    DFX_Text(pFX, _T("[Motor Name]"), m_Motor_Name);
//    DFX_Text(pFX, _T("[Position Name]"), m_Position_Name);
//    DFX_Bool(pFX, _T("[Teach Enable]"), m_Teach_Enable);
//    DFX_Bool(pFX, _T("[Save All Pkg]"), m_Save_All_Pkg);
//    // }}AFX_FIELD_MAP
//}

void CMotorRS::GetAllFieldsValue()
{
    m_rs.GetFieldValue(_T("ID"), m_ID);
    m_rs.GetFieldValue(_T("Module Name"), m_Module_Name);
    m_rs.GetFieldValue(_T("Motor Name"), m_Motor_Name);
    m_rs.GetFieldValue(_T("Position Name"), m_Position_Name);
    m_rs.GetFieldValue(_T("Teach Enable"), m_Teach_Enable);
    m_rs.GetFieldValue(_T("Save All Pkg"), m_Save_All_Pkg);
}

CString CMotorRS::GetConnectionString(CString csDbName)
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
    strConnection = strConnectionFormat + csDbName;

    return strConnection;
}

bool CMotorRS::Open(std::string sqlQuery)
{
    CString strConnection = _T("");
    //
    strConnection = GetConnectionString(GetDefaultDBName());
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

//  set the motor position name from Motor and MotorSubField table
void CMotorRS::SetMtrPosName(stModule *module)
{
    CADODatabase db;
    try
    {
        db.Open(GetConnectionString("d:\\machine\\database\\mitmc.mdb")); // open db

        CString csSQL;

        csSQL.Format("SELECT Motor.*, MotorSubField.SubFieldPosName FROM MotorSubField "
                     "RIGHT JOIN Motor ON MotorSubField.[Position Name] = Motor.[Position Name] "
                     "ORDER BY Motor.ID, MotorSubField.ID");

        CADORecordset rs(&db);
        rs.Open(csSQL);       // open record set

        rs.MoveFirst();                       // move to the first record
        rs.MoveLast();                        // move to the Last record

        int iRecordCnt = rs.GetRecordCount(); // get record count
        rs.MoveFirst();                       // move to the first record

        CString csModuleName; 
        CString csMtrName;    
        
        rs.GetFieldValue("Module Name", csModuleName);
        rs.GetFieldValue("Motor Name", csMtrName);

        int iModule = GetModuleIndex(module);
        int iMtr    = 0;
        int iPos    = 0;
        for (int i = 0; i < iRecordCnt; i++)
        {
            //
            rs.GetFieldValue("Module Name",m_Module_Name);
            if (m_Module_Name != csModuleName)
            {
                // next module
                csModuleName = m_Module_Name;
                iModule      = GetModuleIndex(module);

                // all parameter need to restart
                rs.GetFieldValue("Motor Name",csMtrName);
                iMtr      = 0;
                iPos      = 0;

                if (iModule < -1 || iModule >= MAX_MODULE)
                {
                    ASSERT(FALSE);
                    return;
                }
            }

            rs.GetFieldValue("Motor Name", m_Motor_Name);
            if (m_Motor_Name != csMtrName)
            {
                csMtrName = m_Motor_Name;
                iMtr++;
                if (iMtr < -1 || iMtr >= MAX_MOTOR)
                {
                    ASSERT(FALSE);
                    return;
                }

                iPos = 0;
            }

            CString csVal;
            rs.GetFieldValue("Position Name", csVal);
            csVal          = "Pos: " + csVal;
            CString csSubFieldName;
            rs.GetFieldValue("SubFieldPosName", csSubFieldName);

            if (!csSubFieldName.IsEmpty()) // got sub field position
            {
                if (iPos == 0 || m_vecMtrPos[iModule][iMtr].at(iPos - 1).Find(csVal) == -1)
                {
                    m_vecMtrPos[iModule][iMtr].push_back(csVal);
                }

                csVal += ", Sub Field Pos: " + csSubFieldName;
            }

            m_vecMtrPos[iModule][iMtr].push_back(csVal);
            rs.MoveNext(); // move to the next record
        }

        if (rs.IsOpen())   // if recordset is opened before
        {
            rs.Close();    // close recordset
        }
    }
    //catch (CDaoException* DaoEx)
    //{
    //    //  prompt error
    //    CPromptExceptionMessage::promptDaoException(DaoEx, "Get Motor Position value from MitMc.mdb");
    //    DaoEx->Delete(); // must delete to avoid memory leak!
    //}
    catch (CMemoryException* MemEx)
    {
        //  prompt error
        //CPromptExceptionMessage::promptMemoryException(MemEx, "MitMc.mdb");
        MemEx->Delete(); // must delete to avoid memory leak!
    }
    catch (CException *e)
    {
        e->ReportError();
        e->Delete();            // must delete to avoid memory leak!
    }

    if (db.IsOpen()) // if database is opened before
    {
        db.Close();             // close database
    }
}

// Function to init motor module to maintenance page
void CMotorRS::InitMotorModule(stModule *module)
{
    // !!! Note : Need to order BY ID for proper update of "module" structure !!!
    std::string strSQL = _T("SELECT * FROM [Motor] ORDER BY ID"); // select all fields from Motor table // ORDER BY [Module Name] ASC");

    Open(strSQL);            // Creates a new dynaset recordset
    if (m_rs.IsEOF())                           // check if recordset contains records
    {
        //CMessageBox Box("No record in motor table");
        //Box.DoModal();                     // call modal dialog box to prompt the above message
        Close();                           // close recordset
        return;
    }

    //_unlink("d:\\temp\\motorRS.xml");
    //m_rs.SaveAsXML("d:\\temp\\motorRS.xml");

    m_rs.MoveFirst();                           // move to first record to set the orignal of the db pointer
    m_rs.MoveLast();                            // move to last record
    int iRecordCount = m_rs.GetRecordCount();   // get record count after move to last reord
    m_rs.MoveFirst();                           // move to first record
    // ado2
    GetAllFieldsValue();
    //

    initNoOfMtr(module);                   // initialised no of motor to 0
    int iModule = GetModuleIndex(module);
    if (iModule == -1)
    {
        CString     csMessage = m_Module_Name + " Module not found in motor table";
        //CMessageBox Box(csMessage);
        //Box.DoModal(); // call modal dialog box to prompt the above message
        Close();       // close recordset
        return;
    }

    int iMtrArrayNo = 0;
    int iPos        = 0;

    m_vecMtrName.clear();
    m_vecMtrName.reserve(MAX_MC_MOTOR);

    m_iNoOfModule             = 1;
    m_csModuleName[iModule]   = m_Module_Name;                                              // store module name
    m_vecMtrName.push_back(m_Motor_Name);                                               // store motor name

    module[iModule].csName                                   = m_Module_Name;               // set module name
    module[iModule].Motor[iMtrArrayNo]                       = new CMtrProfile;
    module[iModule].Motor[iMtrArrayNo]->m_csMotorName        = m_Motor_Name;                // set motor name
    module[iModule].Motor[iMtrArrayNo]->m_bTeachEnable[iPos] = m_Teach_Enable;              // set motor teachable
    module[iModule].Motor[iMtrArrayNo]->m_csMotorPos[iPos++] = m_Position_Name;             // set position name

    for (int i = 1; i < iRecordCount; i++)
    {
        m_rs.MoveNext();                                                                         // move to next record
        // ado2
        GetAllFieldsValue();
        //
        if (module[iModule].csName == m_Module_Name)                                        // same module
        {
            if (module[iModule].Motor[iMtrArrayNo]->m_csMotorName == m_Motor_Name)          // same motor
            {
                module[iModule].Motor[iMtrArrayNo]->m_bTeachEnable[iPos] = m_Teach_Enable;  // set motor teachable
                module[iModule].Motor[iMtrArrayNo]->m_csMotorPos[iPos++] = m_Position_Name; // set position name
            }
            else                                                                            // different motor
            {
                module[iModule].Motor[iMtrArrayNo]->nNoOfPos = iPos;                        // set no of pos in one motor

                iPos = 0;
                iMtrArrayNo++;

                module[iModule].Motor[iMtrArrayNo]                       = new CMtrProfile;
                module[iModule].Motor[iMtrArrayNo]->m_csMotorName        = m_Motor_Name;    // set motor name
                module[iModule].Motor[iMtrArrayNo]->m_bTeachEnable[iPos] = m_Teach_Enable;  // set motor teachable
                module[iModule].Motor[iMtrArrayNo]->m_csMotorPos[iPos++] = m_Position_Name; // set position name

                m_vecMtrName.push_back(m_Motor_Name);                                   // store motor name
            }
        }
        else                                                                                // different module
        {
            module[iModule].nNoOfMotor                   = iMtrArrayNo + 1;                 // set no of motor in the module
            module[iModule].Motor[iMtrArrayNo]->nNoOfPos = iPos;                            // set no of pos in the motor

            if (module[iModule].Motor[iMtrArrayNo]->m_csMotorName != m_Motor_Name)          // different motor
            {
                m_vecMtrName.push_back(m_Motor_Name);                                   // store motor name
            }

            iPos        = 0;
            iMtrArrayNo = 0;
            iModule     = GetModuleIndex(module);
            m_iNoOfModule++;
            m_csModuleName[iModule] = m_Module_Name;

            module[iModule].csName                                   = m_Module_Name;   // set module name
            module[iModule].Motor[iMtrArrayNo]                       = new CMtrProfile;
            module[iModule].Motor[iMtrArrayNo]->m_csMotorName        = m_Motor_Name;    // set motor name
            module[iModule].Motor[iMtrArrayNo]->m_bTeachEnable[iPos] = m_Teach_Enable;  // set motor teachable
            module[iModule].Motor[iMtrArrayNo]->m_csMotorPos[iPos++] = m_Position_Name; // set position name
        }
    }

    module[iModule].nNoOfMotor                   = iMtrArrayNo + 1;                     // set no of motor in the last module
    module[iModule].Motor[iMtrArrayNo]->nNoOfPos = iPos;                                // set no of pos in the last motor module

    // move back to first to update m_Module_Name
    m_rs.MoveFirst();
    // ado2
    GetAllFieldsValue();
    //
    Close();                                                                            // close recordset
    SetMtrPosName(module);

    CMtrProfileRS profileRS;
    profileRS.InitMtrProfile(module);
}

// initialised no of motor to 0
void CMotorRS::initNoOfMtr(stModule *module)
{
    for (int i = 0; i < MAX_MODULE; i++)
    {
        module[i].nNoOfMotor = 0;
    }
}

// return the module index
int CMotorRS::GetModuleIndex(stModule *module)
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

// return no of module in motor table
int CMotorRS::GetNoOfModule()
{
    CString csModuleName;

    std::string strSQL = _T("SELECT * FROM [Motor] ORDER BY [Module Name]"); // select all fields from Motor table // ORDER BY [Module Name] ASC");

    try
    {
        Open(strSQL);                               // Creates a new dynaset recordset

        if (m_rs.IsEOF())                                              // check if recordset contains records
        {
            //CMessageBox Box(CMultiLanguage::GetMessageText(CMultiLanguage::NO_RECORDS_FOUND_IN_MOTOR_TABLE));
            //Box.DoModal();                                        // call modal dialog box to prompt the above message
            Close();                                              // close recordset
            return 0;                                             // return no modules
        }
        else
        {
            m_rs.MoveFirst();                                          // move to first record to set the orignal of the db pointer
            m_rs.MoveLast();                                           // move to last record
            int iRecordCount = m_rs.GetRecordCount();                  // get record count after move to last reord
            //      TRACE("iRecordCount = %d\n",iRecordCount);
            m_rs.MoveFirst();                                          // move to first record
            // ado2
            GetAllFieldsValue();
            //
            m_iNoOfModule = 1;
            csModuleName  = m_Module_Name;                        // store module name

            for (int i = 1; i < iRecordCount; i++)
            {
                //          TRACE("module[m_iNoOfModule].csName = %s\n",module[iModule].csName);
                m_rs.MoveNext();                        // move to next record
                // ado2
                GetAllFieldsValue();
                //
                if (csModuleName != m_Module_Name) // same module
                {
                    m_iNoOfModule++;
                    csModuleName = m_Module_Name;  // store module name
                }
            }
        }
    }
    //catch (CDaoException* DaoEx)    // catch CDaoException
    //{
    //    // prompt error
    //    CPromptExceptionMessage::promptDaoException(DaoEx, "CMotorRS::GetNoOfModule()");
    //    DaoEx->Delete();            // must delete to avoid memory leak!
    //}
    catch (CMemoryException* MemEx) // catch Memory exception
    {
        // prompt error
        //CPromptExceptionMessage::promptMemoryException(MemEx, "CMotorRS::GetNoOfModule()");
        MemEx->Delete();            // must delete to avoid memory leak!
    }
    catch (CException *e)
    {
        e->ReportError();
        e->Delete(); // must delete to avoid memory leak!
    }
    Close();         // close recordset
    return m_iNoOfModule;
}

// return module name in motor table
CString* CMotorRS::GetAllModuleName()
{
    return m_csModuleName; // return module name in motor table
}

// return motor name in motor table
std::vector<CString> CMotorRS::GetAllMtrName()
{
/*  for (int i = 0; i < m_iNoOfMtr; i++)
        TRACE("m_csMtrName[%d] = %s\n",i,m_csMtrName[i]);*/
    return m_vecMtrName; // return module name in motor table
}

// return no of motor
int CMotorRS::GetNoOfMtr()
{
    return m_vecMtrName.size(); // return no of motor in motor table
}

//  get motor and pos name
bool CMotorRS::GetMtrAndPosName(stModule *module, int iMtrNo, int iMtrPos, CString &csMtrName, CString &csMtrPos)
{
    if (iMtrPos < -1) // invalid motor position
    {
//      display diagnostic error message : show file name and line number
        ASSERT(0);
        return false;
    }

    // have to use this method by comparing the index as read and assign thru database order
    for (int i = 0; i < MAX_MODULE; i++)
    {
        for (int j = 0; j < module[i].nNoOfMotor; j++)
        {
            if (module[i].Motor[j]->m_iMotorPosArrayIndex == iMtrNo)
            // 21 Sep 04 end Gary
            {
                csMtrName = module[i].Motor[j]->m_csMotorName;   // store motor name

                if (iMtrPos == -1)                               // home position
                {
                    csMtrPos = "Home";
                }
                else if (iMtrPos < m_vecMtrPos[i][j].size())  // valid motor position
                {
                    csMtrPos = m_vecMtrPos[i][j].at(iMtrPos); // store motor position name
                }
                else // motor position > num of mtr pos store in m_csaMtrPos
                {
                    TRACE("Pos number = %d Mtr Pos No : %d\n",
                          m_vecMtrPos[i][j].size(), iMtrPos);

                    return false;
                }

                return true; // motor position found return to calling function
            }
        }
    }

//  display diagnostic error message : show file name and line number
    ASSERT(0);
    return false;
}

// get motor pos name.
int CMotorRS::GetMtrPosNo(stModule *module, CString csModuleName, int iMtrNo,
                          CString csMtrPos, CString csSubPosName)
{
    m_Module_Name = csModuleName;
    int iModule       = GetModuleIndex(module);
    int iPosArraySize = m_vecMtrPos[iModule][iMtrNo].size();
    for (int i = 0; i < iPosArraySize; i++)
    {
        auto csPosName = m_vecMtrPos[iModule][iMtrNo].at(i);
        auto iCharPos  = csPosName.Find(csMtrPos);
        if (iCharPos > -1)
        {
            if (!csSubPosName.IsEmpty())
            {
                if (csPosName.Find(csSubPosName, iCharPos + csMtrPos.GetLength() + 1)  > -1)
                {
                    return i;
                }
            }
            else
            {
                return i;
            }
        }
    }

    ASSERT(FALSE);
    return 0;
}

// get Save all pkg
BOOL CMotorRS::GetSaveAllPkg(CString csPosName)
{
    BOOL bSaveAllPkg = FALSE;
    try
    {
        CString csSQL;
        csSQL.Format("SELECT * FROM [Motor] WHERE [Position Name] = \"%s\"", csPosName);

        m_rs.Open(csSQL); // Creates a new dynaset recordset
        bSaveAllPkg = m_Save_All_Pkg;
    }
    //catch (CDaoException* DaoEx)    // catch CDaoException
    //{
    //    // prompt error
    //    CString cMsg;
    //    cMsg.Format("CMotorRS::GetSaveAllPkg() position name = %s", csPosName);
    //    CPromptExceptionMessage::promptDaoException(DaoEx, cMsg);
    //    DaoEx->Delete();            // must delete to avoid memory leak!
    //}
    catch (CMemoryException* MemEx) // catch Memory exception
    {
        // prompt error
        CString cMsg;
        cMsg.Format("CMotorRS::GetSaveAllPkg() position name = %s", csPosName);
        //CPromptExceptionMessage::promptMemoryException(MemEx, cMsg);
        MemEx->Delete(); // must delete to avoid memory leak!
    }
    catch (CException *e)
    {
        e->ReportError();
        e->Delete(); // must delete to avoid memory leak!
    }
    Close();         // close recordset
    return bSaveAllPkg;
}

// Close Recordset and close database
void CMotorRS::Close()
{
    if (m_rs.IsOpen())                             // if recordset is open
    {
        m_rs.Close();               // close recordset
    }

    if (m_db.IsOpen()) // if database is open
    {
        m_db.Close();                 // close database
    }
}

/////////////////////////////////////////////////////////////////////////////
// CMotorRS diagnostics

#ifdef _DEBUG
void CMotorRS::AssertValid() const
{
    //CDaoRecordset::AssertValid();
}

void CMotorRS::Dump(CDumpContext& dc) const
{
    //CDaoRecordset::Dump(dc);
}

#endif // _DEBUG

int CMotorRS::InitModule(stModule *module)
{
    int iRecordCount = 0;

    CADODatabase  dbModule;
    CADORecordset RecordSet(&dbModule);

    try
    {
        dbModule.Open(this->GetConnectionString("d:\\machine\\database\\mitmc.mdb"));

        CString csSQL;
        csSQL.Format("SELECT * FROM [Module] ORDER BY ID");
        RecordSet.Open(csSQL);

        if (RecordSet.IsEOF())
        {
            //CMessageBox Box("No Module Found"));
            //Box.DoModal();
        }
        else
        {
            RecordSet.MoveFirst();                     // move to first record to set the orignal of the db pointer
            RecordSet.MoveLast();                      // move to last record
            iRecordCount = RecordSet.GetRecordCount(); // get record count after move to last reord
//          TRACE("\niRecordCount=%d",iRecordCount);
            RecordSet.MoveFirst();                     // move to first record

            for (int i = 0; i < iRecordCount; i++)
            {
                RecordSet.GetFieldValue("Module Name", module[i].csName);

                RecordSet.MoveNext();
//              TRACE("\nm_stModule[%d].csName=%s",i,m_stModule[i].csName);
            }
        }
    }
    //catch (CDaoException* DaoEx)
    //{
    //    //  prompt error
    //    CPromptExceptionMessage::promptDaoException(DaoEx, "read module to MitMc.mdb",
    //                                                "Module Table");
    //    DaoEx->Delete(); // must delete to avoid memory leak!
    //}
    catch (CMemoryException* MemEx)
    {
        //  prompt error
        //CPromptExceptionMessage::promptMemoryException(MemEx, "MitMc.mdb");
        MemEx->Delete();
    }
    catch (CException *e)
    {
        e->ReportError();
        e->Delete();        // must delete to avoid memory leak!
    }
    if (RecordSet.IsOpen()) // is record set is opened
    {
        RecordSet.Close();  // close record set
    }

    dbModule.Close();       // close data base

    return iRecordCount;
}
