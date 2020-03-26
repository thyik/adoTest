#pragma once

#pragma warning (disable: 4146)

// CG : In order to use this code against a different version of ADO, the appropriate
// ADO library needs to be used in the #import statement
#pragma message ("Make sure you go to Tools.Options.Directories.Library files and add the paths to msado15.dll and msjro.dll will usually be in C:\\Program Files\\Common Files\\System\\ado")
//#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF", "EndOfFile")
//#import "C:\Program Files\Common Files\System\ado\MSJRO.DLL" no_namespace rename("ReplicaTypeEnum", "_ReplicaTypeEnum") 

//#import <msado15.dll> rename("EOF", "EndOfFile")
#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
              rename("EOF", "EndOfFile")

// msjro : for jet database compact feature
//#define _JETENGINE
#ifdef _JETEGINE
//#import <MSJRO.DLL> no_namespace rename("ReplicaTypeEnum", "_ReplicaTypeEnum") 
//#import "C:\Program Files (x86)\Common Files\System\ado\MSJRO.DLL" no_namespace rename("ReplicaTypeEnum", "_ReplicaTypeEnum") 
#endif _JETENGINE
#pragma warning (default: 4146)