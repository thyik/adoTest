#pragma once
#include <string>
#ifdef  BUILD_UTILITYDLL
#define     UTILITYDLL  __declspec(dllexport)
#else
#define     UTILITYDLL  __declspec(dllimport)
#endif


class /*UTILITYDLL*/ CFileLocation
{
public:
    enum FilesEnum
    {
        GUILDLL_INI_FILE,  ///< enum value for GuiDll.ini.
        HARDWARE_INI_FILE, ///< enum value for HardWare.ini.
        RUNTIME_INI_FILE,  ///< enum value for RunTime.ini.
        CONFIG_INI_FILE,   ///< enum value for Config.ini.
        DEBUG_LOG,         ///< enum value for dbg.log.
        DEBUG_TIMING_LOG,  ///< enum value for DebugTiming.log.
        GUIEVENT_LOG,      ///< enum value for GUIEvent.log.
        MACHINE_DB_FILE,
        JAM_DB_FILE,
        PACKAGE_DB_FILE,
        LOTINFO_DB_FILE,
        LANGUAGE_DB_FILE,
        MESSAGE_DB_FILE,
        MAX_FILE_LOCATION ///< max number of files.
    };

    const char           *GetLocation(FilesEnum f);
    void                  UpdatePath(FilesEnum f, const std::string &path);
    void                  Reset();
    static CFileLocation &instance();
    static std::string    GetCurrentDirectory();
    static std::string    GetSubPathInCurrentDirectory(const std::string &subPath);
    static std::string    BuildPath(const std::string &path, const std::string &subPath);
    static std::string    GetFileVersion(const std::string &filename);
    static std::string    GetCurrentModulePath();
    static std::string    PathRemoveFileSpec(const std::string &filename);
    static std::string    PathStripPath(const std::string &filename);
    static bool           Exists(const std::string &path);

private:
    CFileLocation(void);
    ~CFileLocation(void);
    void LoadLocations();
    std::string m_strFilePath[MAX_FILE_LOCATION];
};
