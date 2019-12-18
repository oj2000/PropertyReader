#pragma once
#include <string>
#include <fstream>
#include <windows.h>

namespace FileReader
{
class CIniFileObj
{
public:
    explicit CIniFileObj(wchar_t* strFname);

    bool OpenFileToRead();
    bool OpenFileAndDisplayContents();
    bool WaitForFileChangesAndExitAfterDeletion();

private:
    wchar_t m_strIniFilename[MAX_PATH];
    std::wifstream m_IniFile;
};
} // IniFile::FileReader