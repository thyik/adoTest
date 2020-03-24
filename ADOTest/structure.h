#ifndef _STRUCTURE
#define _STRUCTURE
//#include "Input.h"
//#include "Output.h"
//#include "Motor.h"

//#include <afxwin.h>  // MFC core and standard components
//#include <afxext.h>  // MFC extensions
//#include <afxdisp.h> // MFC OLE automation classes

#include <afxcmn.h>  // MFC support for Windows Common Controls
//#include <afxdisp.h>
//#include <afxmt.h>
//#include "windef.h"
//#include <afxcoll.h>

// Input Card
#define     PCI_7433_ADLINK     0

// Input & Output Card
#define     PCI_7296_ADLINK     1
#define     PCI_9112_ADLINK     2
#define     PCI_7851_ADLINK     3
#define     PCI_7432_ADLINK     4 // 32 in, 32 out
#define     PCI_9222_ADLINK     5 // AO, AI, Encoder, DI, DO card
#define     PCI_7856_HSL_ADLINK 6
#define     MAX_INPUT_CARD_TYPE 7

// 7856 Port Definition
#define     PCI_7856_HSL_PORT       0
#define     PCI_7856_MOTIONNET_PORT 1

// Output Card
#define     PCI_7434_ADLINK         0
#define     MAX_OUTPUT_CARD_TYPE    MAX_INPUT_CARD_TYPE

// note that the output array size is set based on 6*16*144 = 13824 output pts total
// 1 card type can have max 16*144=2304 output pts
#define MAX_CARD_TYPE               MAX_OUTPUT_CARD_TYPE
#define MAX_CARD_FOR_EACH_TYPE      16
#define MAX_POINTS_FOR_EACH_CARD    144

// for remote IO 7851, max IO 63*32=2016. This value must be smaller than 2304 (as above).
// for remote IO 7856 (HSL), max IO 63*32=2016. This value must be smaller than 2304 (as above).
#define MAX_SLAVE_ID                    63
#define MAX_POINTS_FOR_EACH_SLAVE_ID    32

// ETEL
#define MAX_ETEL_AXIS   32

enum CARD_TYPE
{
    INPUT_CARD,
    OUTPUT_CARD,
    MOTION_CARD,
    ENCODER_CARD
};

enum INPUT_STATE
{
    INPUT_UNKNOWN,
    INPUT_ON,
    INPUT_OFF,
};

enum INPUT_CARD_INIT_RESULT
{
    INPUT_CARD_INIT_FAIL,
    INPUT_CARD_INIT_PASS,
};

enum INPUT_CARD_CLOSE_RESULT
{
    INPUT_CARD_CLOSE_FAIL,
    INPUT_CARD_CLOSE_PASS,
};

enum OUTPUT_CARD_INIT_RESULT
{
    OUTPUT_CARD_INIT_FAIL,
    OUTPUT_CARD_INIT_PASS,
};

enum OUTPUT_CARD_CLOSE_RESULT
{
    OUTPUT_CARD_CLOSE_FAIL,
    OUTPUT_CARD_CLOSE_PASS,
};

//
#define MAX_MODULE  32
#define MAX_MODULE_INPUT    11 // 16
#define MAX_MODULE_OUTPUT   15
#define MAX_MODULE_MOTOR    16

#define MAX_MC_MOTOR            64          // Max motor per machine
#define MAX_MOTOR               50
#define MAX_MOTOR_MODULE_ARRAY  MAX_MOTOR
#define MAX_IO_MODULE_ARRAY     75
#define MAX_MOTOR_POS           30

// motor profile data structure
class /*IODLLCLASS*/ CMtrProfile
{
public:
    int nNoOfPos;

    CString m_csMotorName;
    int     m_iMtrNo;         // motor no wrt the type of card used
    int     m_iMtrType;       // servo or stepper, with or without encoder feedback
//  int     m_iUpr;         // unit per motor revolution
//  int     m_iPPR;         // pulse per revolution
//  int     m_iEPPR;        // encoder pulse per revolution
    double m_dUpr;            // unit per motor revolution
    double m_dPPR;            // pulse per revolution
    double m_dEPPR;           // encoder pulse per revolution
    int    m_iPosDir;         // positive dir is CW or CCW
    int    m_iClk;            // OUT/DIR or CW/CCW type
    int    m_iHomeMode;       // homing mode
    int    m_iHomeDir;        // homing dir
    int    m_iIndexLogic;     // index or z pulse signal logic from the encoder
    int    m_iALM;            // motor driver alarm signal logic
    int    m_iINP;            // in-position logic from motor driver
    int    m_iPosEL;          // positive limit sensor signal logic
    int    m_iNegEL;          // negative limit sensor signal logic
    int    m_iORGI;           // home sensor signal logic
    int    m_iRDY;            // motor driver Ready signal logic
    int    m_iSON;            // motor enable signal logic
    int    m_iCLR;            // motor driver clear alarm signal logic

    double m_dConst;          // constant speed, must be double
    double m_dStartUp;        // start speed, must be double
    double m_dCruise;         // max speed, must be double
    double m_dAcce;           // acceleration
    double m_dDec;            // deceleration
    int    m_iCardType;       // card type, eg 8134
    double m_dSAcce;          // s-curve profile acceleration
    double m_dSDec;           // s-curve profile deceleration
    double m_dHomeLeaveSteps; // leave steps for homing

    int m_iEncoderType;       // type of encoder, 1xAB, 2xAB, 4xAB, CW/CCW

    double m_dPosSoftLimit;   // positive Software Limit
    double m_dNegSoftLimit;   // negative Software Limit

    int m_iCardNo;

    int m_iMotorPosArrayIndex;

    CString m_csMotorPos[MAX_MOTOR_POS];
    BOOL    m_bTeachEnable[MAX_MOTOR_POS];
};

class /*IODLLCLASS*/ COutputSetting
{
public:
// configuration
    COutputSetting() :
        m_szName(nullptr),
        m_szModuleName(nullptr),
        m_bLogic(FALSE),
        m_nPointNo(0),
        m_nSlaveID(0),
        m_nPtrNo(0),
        m_nBoardType(0),
        m_bEnableCheck(FALSE),
        m_nCardNo(0)
    {
    }

    LPCTSTR m_szName;
    LPCTSTR m_szModuleName;
    BOOL    m_bLogic;
    int     m_nPointNo;
    int     m_nSlaveID;
    int     m_nPtrNo;
    int     m_nBoardType;
    BOOL    m_bEnableCheck;
    int     m_nCardNo;
};

class /*IODLLCLASS*/ CInputSetting
{
public:
// configuration
    CInputSetting() :
        m_szName(nullptr),
        m_szModuleName(nullptr),
        m_bLogic(FALSE),
        m_nPointNo(0),
        m_nSlaveID(0),
        m_nPtrNo(0),
        m_nBoardType(0),
        m_nCardNo(0)
    {
    }

    LPCTSTR m_szName;
    LPCTSTR m_szModuleName;
    BOOL    m_bLogic;
    int     m_nPointNo;
    int     m_nSlaveID;
    int     m_nPtrNo;
    int     m_nBoardType;
    int     m_nCardNo;
};

struct stModule
{
    stModule()
    {
        nNoOfInput  = 0;
        nNoOfOutput = 0;
        nNoOfMotor  = 0;
        for (int i = 0; i < MAX_MOTOR_MODULE_ARRAY; i++)
        {
            Motor[i] = nullptr;
        }
    }

    bool HasMotors()
    {
        return nNoOfMotor > 0;
    }

    CString csName;

    CInputSetting Input[MAX_IO_MODULE_ARRAY];
    int nNoOfInput;

    COutputSetting Output[MAX_IO_MODULE_ARRAY];
    int nNoOfOutput;

    CMtrProfile *Motor[MAX_MOTOR_MODULE_ARRAY];
    int nNoOfMotor;
};

// input and output module data structure for Maintenance page
struct stMaintIoModule
{
    CString m_csModuleName;                  // store module name
    CString m_csIoName[MAX_IO_MODULE_ARRAY]; // MAX_MODULE_INPUT]; // store input name
};

// Gary 21Dec10, V1.23.02 for multilanguage motor name and motor position name
// for Maintenance page
struct stMaintMotorModule
{
    CString m_csModuleName;              // store module name
    CString m_csMtrName[MAX_MOTOR];      // store motor name
    CStringArray m_csaMtrPos[MAX_MOTOR]; // store Motor Position Name
};

// user change accordingly to suit the machine requirement
struct stLotInfo
{
    CString csLotStarted;
    CString csLotName;
    long lLotQty;
    CString csShiftID;
    CString csOperatorID;
    CString csLotStartTime;
    CString csRecipeName;
    CString csPackageName;
};

struct stRecipeInfo
{
    CString csRecipeName;
    CString csPackageName;
};

struct stPackageInfo
{
    CString csPackageName;
    int nPackageID;
};

// Light Config
typedef struct _LIGHT_CONFIG {
    int iRed;
    int iAmber;
    int iGreen;
    int iBuzzer;
    int iLightOnTime;
    int iLightOffTime;
    int iBuzzerOnTime;
    int iBuzzerOffTime;

    // ctor 
    _LIGHT_CONFIG()
    {
        iRed           = 1;
        iAmber         = 1;
        iGreen         = 1;
        iBuzzer        = 0;
        iLightOnTime   = 500;
        iLightOffTime  = 500;
        iBuzzerOnTime  = 500;
        iBuzzerOffTime = 500;

    };

} LIGHT_CONFIG;

//
#endif
