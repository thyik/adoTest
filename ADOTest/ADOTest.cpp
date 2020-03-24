// ADOTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ado2.h"

int _tmain(int argc, _TCHAR* argv[])
{
    CADODatabase AdoDb;
    CString strConnection = _T("");
#ifdef _WIN64
    //strConnection = _T("Provider=Microsoft.ACE.OLEDB.12.0;Persist Security Info=False;"\
    //         "Data Source=D:\\Machine\\Database\\MitPkg.mdb");
    strConnection = _T("Provider=Microsoft.ACE.OLEDB.12.0;Persist Security Info=False;"\
             "Data Source=D:\\Machine\\Database\\MitMc.mdb");
#else
    // for 32-bit process
    strConnection = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=D:\\Machine\\Database\\MitPkg.mdb";
#endif

    AdoDb.SetConnectionString(strConnection);

    if(AdoDb.Open())
    {
        CADORecordset rs(&AdoDb);

//        if(rs.Open("SELECT * FROM [MotorSpeed] ORDER BY [Package Name]", CADORecordset::openQuery))
        //if(rs.Open("SELECT * FROM [MotorSpeed] WHERE [Package Name] = \"Default\"", CADORecordset::openQuery))
        if(rs.Open("SELECT Motor.*, MotorSubField.SubFieldPosName FROM MotorSubField "
                     "RIGHT JOIN Motor ON MotorSubField.[Position Name] = Motor.[Position Name] "
                     "ORDER BY Motor.ID, MotorSubField.ID", CADORecordset::openQuery))
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
                rs.GetFieldValue("Module Name", csValue);
                rs.GetFieldValue("Motor Name", csValue);
                rs.GetFieldValue("Position Name", csValue);
                rs.GetFieldValue("SubFieldPosName", csValue);
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

    return 0;
}

