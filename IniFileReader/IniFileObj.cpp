#include <iostream>
#include <windows.h>
#include <pathcch.h>
#include "IniFileObj.h"

namespace FileReader
{
CIniFileObj::CIniFileObj(wchar_t* strFname)
{
    wcscpy_s(m_strIniFilename, MAX_PATH, strFname);
}

bool CIniFileObj::OpenFileAndDisplayContents()
{
    bool bRetVal = true;
    m_IniFile.open(m_strIniFilename);

    if (m_IniFile.is_open()) {
        std::wstring strLine;

        while (std::getline(m_IniFile, strLine)) {
            std::wcout << strLine;

            // 1 find section
            std::wstring::size_type uFoundStart = strLine.find('[');
            
            if (uFoundStart != std::wstring::npos) {
                std::wstring::size_type uFoundEnd = strLine.find(']', uFoundStart + 1);

                if (uFoundEnd != std::wstring::npos) {
                    std::wstring strSection = strLine.substr(uFoundStart + 1, uFoundEnd - uFoundStart - 1);
                    std::wcout << '\t' << "section: " << strSection << '\n';
                }
            }
            else {
                // 2 read key
                std::wstring::size_type uKeyEnd = std::wstring::npos;
                std::wstring::size_type uDelimPos = strLine.find_first_of(L" =");

                if (uDelimPos != std::wstring::npos) {
                    if (strLine[uDelimPos] == ' ') {
                        std::wstring::size_type uEq = strLine.find('=', uDelimPos + 1);

                        if (uEq != std::wstring::npos) {
                            uKeyEnd = uDelimPos;
                        }
                        else {
                            bRetVal = false;
                        }
                    }
                    else
                    if (strLine[uDelimPos] == '=') {
                        uKeyEnd = uDelimPos;
                    }
                    
                    std::wstring strKey = strLine.substr(0, uKeyEnd);
                    std::wcout << '\t' << "key: " << strKey;
                }
                else {
                    continue;
                }

                // 3 read value
                std::wstring strVal = strLine.substr(uKeyEnd + 1);
                std::wcout << '\t' << "value: " << strVal << '\n';
            }
        }

        m_IniFile.close();    // w/o calling close() the file can't be deleted

        return bRetVal;
    }
    else
    {
        return false;
    }
}

bool CIniFileObj::WaitForFileChangesAndExitAfterDeletion()
{
    bool bRC = false;
    HANDLE  hNotify;
    DWORD   dwWaitResult;
    wchar_t acPath[MAX_PATH];

    wcscpy_s(acPath, MAX_PATH, m_strIniFilename);
    PathCchRemoveFileSpec(acPath, MAX_PATH);

    hNotify = FindFirstChangeNotification(acPath, FALSE, FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME);

    for (;;) {
        std::wcout << L"Waiting for changes (delete .ini file to exit)" << '\n';
        dwWaitResult = WaitForSingleObject(hNotify, INFINITE);
        // I get 2 notifications for each change.
        // This is entirely normal. A change to a file usually involves a change to the file data as well as a change to the directory entry.
        // Metadata properties like the file length and the last write date are stored there.

        if (WAIT_OBJECT_0 != dwWaitResult) break;

        if (0xFFFFFFFF == GetFileAttributes(m_strIniFilename)) {    // file removed => exit
            bRC = true;
            break;
        }
        else {
            std::wcout << '\n' << L"The changed contents" << '\n';
            OpenFileAndDisplayContents();

            FindNextChangeNotification(hNotify);
        }
    }

    std::wcout << L"Terminating due to removed file" << '\n';
    FindCloseChangeNotification(hNotify);

    return bRC;
}
} // namespace FileReader