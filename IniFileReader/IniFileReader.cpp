#include <iostream>
#include "IniFileObj.h"

int main()
{
    wchar_t strFname[MAX_PATH] = L"c:\\tmp\\1st.ini";
    FileReader::CIniFileObj iniFile(strFname);

    std::wcout << L"Contents of " << std::wstring(strFname) << '\n';

    if (iniFile.OpenFileAndDisplayContents()) {
        iniFile.WaitForFileChangesAndExitAfterDeletion();
    }
    else {
        std::wcout << L"Error opening file " << std::wstring(strFname) << '\n';
    }
}