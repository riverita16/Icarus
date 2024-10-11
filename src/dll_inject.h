#ifndef DLL_DEFINE_H
#define DLL_DEFINE_H

#include <windows.h>
#include <string>

using namespace std;

class DLLInjector {
public:
    DLLInjector(const wstring &processName, const string &dllPath);
    bool inject();

private:
    wstring processName;
    string dllPath;

    DWORD getProcessId(const wstring &processName);
    HANDLE openTargetProcess(DWORD processId);
    bool injectDLL(HANDLE processHandle);
};

#endif