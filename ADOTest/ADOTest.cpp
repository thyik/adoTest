// ADOTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ado2.h"
#include "MaintenanceInputRS.h"
#include "MaintenanceOutputRS.h"
#include "MotorRS.h"

#include "SharedMemory.h"
#include "FileLocation.h"

#include "ODBCRecordset.h"

int _tmain(int argc, _TCHAR* argv[])
{
    CADODatabase AdoDb;
    CString strConnection = _T("");
#ifdef _WIN64
    //strConnection = _T("Provider=Microsoft.ACE.OLEDB.12.0;Persist Security Info=False;"\
    //         "Data Source=D:\\Machine\\Database\\MitPkg.mdb");
    strConnection = _T("Provider=Microsoft.ACE.OLEDB.12.0;Persist Security Info=False;"\
             "Data Source=D:\\Machine\\Database\\MitPkg.mdb");
#else
    // for 32-bit process
    strConnection = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=D:\\Machine\\Database\\MitPkg.mdb";
#endif

    AdoDb.SetConnectionString(strConnection);

    if(AdoDb.Open())
    {
        CADORecordset rs(&AdoDb);

        // mitpkg.mdb
//        if(rs.Open("SELECT * FROM [MotorSpeed] ORDER BY [Package Name]", CADORecordset::openQuery))
        if(rs.Open("SELECT * FROM [MotorSpeed] WHERE [Package Name] = \"Default\"", CADORecordset::openQuery))

            // mitmc.mdb
        //if(rs.Open("SELECT Motor.*, MotorSubField.SubFieldPosName FROM MotorSubField "
        //             "RIGHT JOIN Motor ON MotorSubField.[Position Name] = Motor.[Position Name] "
        //             "ORDER BY Motor.ID, MotorSubField.ID", CADORecordset::openQuery))
        {
            //
            CString xmlFile = "D:\\Temp\\test.xml";
            _unlink(xmlFile);
            rs.SaveAsXML(xmlFile);
            //
            DWORD count = rs.GetRecordCount();
            while (!rs.IsEOF())
            {
                CString csValue;
                double homeMaxSpd;
                //rs.GetFieldValue("Package Name", csValue);
                //rs.GetFieldValue("Module Name", csValue);
                //rs.GetFieldValue("Motor Name", csValue);
                //rs.GetFieldValue("Position Name", csValue);
                //rs.GetFieldValue("SubFieldPosName", csValue);
                //rs.GetFieldValue("Home Max Speed", homeMaxSpd);
                // eg. : add, update
                //rs.AddNew();
                //rs.SetFieldValue(0, "dataU");
                //rs.SetFieldValue(1, "data01");
                //rs.SetFieldValue(2, (long)51000);
                //COleDateTime time = COleDateTime(2001,6,15, 10, 8, 30);
                //rs.SetFieldValue(3, time);
                //rs.Update();
                //rs.Close()
                //
                rs.MoveNext();
            }
            rs.Close();
        }
      //DoSomething();
      //.
      //.
      //.
        //BOOL test = AdoDb.Execute("select * from MotorSpeed order by [Package Name]");

        AdoDb.Close();
    }
    //
    CMotorRS rsMotor;
    CMaintenanceInputRS rsInput;
    CMaintenanceOutputRS rsOutput;
    stModule module[MAX_MODULE];

    rsMotor.InitModule(module);
    rsMotor.InitMotorModule(module);
    rsMotor.GetNoOfModule();

    rsInput.InitInputModule(module);
    ////
    rsOutput.InitOutputModule(module);
    //

    // release memory
    for (int i = 0; i < MAX_MODULE; i++)
    {
        for (int j = 0; j < MAX_MOTOR_MODULE_ARRAY; j++)
        {
            if (module[i].Motor[j] != NULL)
            {
                delete  module[i].Motor[j];
            }
        }
    }

    for (int i = 0; i < MAX_MODULE; i++)
    {
        module[i].csName.Empty();
        module[i].nNoOfInput  = 0;
        module[i].nNoOfMotor  = 0;
        module[i].nNoOfOutput = 0;

        for (int j = 0; j < MAX_MOTOR_MODULE_ARRAY; j++)
        {
            module[i].Motor[j] = NULL;
        }
    }

    // shm
    typedef struct _stShmData {
        int data1;
        double data2;
    } stShmData;

    TCHAR  szName[] = TEXT("Global/MotorIO");

    off_t size = 1 * sizeof(stShmData);

    if (size <= 0)
    {
        size = sizeof(stShmData);
    }

// round up to the OS page size.

    stShmData *pData = NULL;
    //TRACE("\n\n\n+++ MotionThreadFn sizeof(CMtrStatus) = %d  m_bEnabShm =%d size of sharedram %d m_iNoOfMtr=%d\n", sizeof(CMtrStatus), size, run->m_iNoOfMtr);

    // the following object meant not to ve unmapped
    CSharedMemory shm(szName, size, false);
    pData = static_cast<stShmData *>(shm.getDirectMap());
    ASSERT(pData != NULL);
    TRACE("SHM MMF pData= (0x%X).\n", (void*)pData);


    std::string utilityTestPath = CFileLocation::GetCurrentModulePath();
    CFileLocation::PathRemoveFileSpec(utilityTestPath);

    // sample odbc code
    CDatabase db;
    //    This connect string will pop up the ODBC connect dialog
    //CString        cConnect = "ODBC;";
    //    This connect string will directly open & connect to the mdb
    //      either of this will work. Can check the detail of the string via "ODBC Data Source" configuration
    //      under [User DSN] / [Drivers]
    // this connection string works for x64 process
    CString        cConnect = "ODBC;DSN=MS Access Database;DBQ=d:\\machine\\database\\mitpkg.mdb";
    //CString        cConnect = "ODBC;Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=d:\\machine\\database\\mitpkg.mdb";
    //
    // this connection string works for 32-bit process
    //CString        cConnect = "ODBC;Driver={Microsoft Access Driver (*.mdb)};DBQ=d:\\machine\\database\\mitpkg.mdb";
    db.Open( NULL,                //    DSN
         FALSE,                //    Exclusive
         FALSE,                //    ReadOnly
         cConnect,            //    ODBC Connect string
         TRUE                //    Use cursor lib
       );

    COleDateTime    dOrderDate;
    CODBCRecordset rs(&db);
    //BOOL bStatus = rs.Open( "SELECT * FROM [MotorSpeed] WHERE [Package Name] = \"Default\"" );
    // ODBC query cannot work with double quote enclosing"
    BOOL bStatus = rs.Open( "SELECT * FROM [MotorSpeed] WHERE [Package Name] = 'Default'" );
    //BOOL bStatus = rs.Open( "SELECT * FROM [MotorSpeed]" );

    while (!rs.IsEOF())
    {
        CString csValue = rs.Field("Motor Name");
        TRACE("Item= %s\n", csValue);
        rs.MoveNext();
    }
    rs.Close();
    return 0;
}

