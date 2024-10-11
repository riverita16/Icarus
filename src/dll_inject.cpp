#define UNICODE
#define _UNICODE

#include "dll_inject.h"
#include <tlhelp32.h>
#include <iostream>
#include <fstream>

using namespace std;

DLLInjector::DLLInjector(const wstring &processName, const string &dllPath): processName(processName), dllPath(dllPath) {}

DWORD DLLInjector::getProcessId(const wstring &processName) {
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    // get snapshot of procs
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &processEntry)) {
        do {
            wstring exeFileName(processEntry.szExeFile, wcsnlen(processEntry.szExeFile, MAX_PATH));
            if (processName == exeFileName) {
                CloseHandle(snapshot);
                return processEntry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    // proc not found
    CloseHandle(snapshot);
    return 0;
}

HANDLE DLLInjector::openTargetProcess(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

    if (processHandle == NULL) {
        DWORD error = GetLastError();
        cerr << "Failed to get proc handle" << error << endl;
    }

    return processHandle;
}

bool DLLInjector::injectDLL(HANDLE processHandle) {
    LPVOID allocMem = VirtualAllocEx(processHandle, NULL, dllPath.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (allocMem == NULL) {
        cerr << "Failed to alloc mem in target proc" << endl;
        return false;
    }

    if (!WriteProcessMemory(processHandle, allocMem, dllPath.c_str(), dllPath.length() + 1, NULL)) {
        cerr << "Failed to write DLL into target proc" << endl;
        return false;
    }

    HMODULE hkernel32 = GetModuleHandle(L"kernel32.dll");
    if (hkernel32 == NULL) {
        cerr << "Failed to get handle to kernel32.dll" << endl;
        return false;
    }

    LPVOID loadLibAddr = (LPVOID)GetProcAddress(hkernel32, "LoadLibraryA");
    if (loadLibAddr == NULL) {
        cerr << "Failed to get addr of LoadLibraryA" << endl;
        return false;
    }

    if (processHandle == NULL) {
        cerr << "NULL process handle" << endl;
    }

    // telling the thread to run LoadLibraryA("C:\\Users\\Alanis\\Documents\\CS\\Icarus\\testing\\test.dll")
    HANDLE hThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, allocMem, 0, NULL);
    if (hThread == NULL) {
        DWORD error = GetLastError();
        cerr << "Failed to create remote thread in target proc: " << error << endl;
        return false;
    }

    // wait for thread to finish exec
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    return true;
}

bool DLLInjector::inject() {
    DWORD procId = getProcessId(processName);
    if (procId == 0) {
        cerr << "Failed to find target proc" << endl;
        return false;
    }

    HANDLE procHandle = openTargetProcess(procId);
    if (procHandle == NULL) {
        cerr << "Failed to open target proc" << endl;
        return false;
    }

    if (!injectDLL(procHandle)) {
        cerr << "DLL injection failed" << endl;
        CloseHandle(procHandle);
        return false;
    }

    CloseHandle(procHandle);
    return true;
}




/**
 * Reflective would require additional work
 * 
 * Would include something like this:
 * read DLL file into buffer for direct injection
vector<char> readDLL(const string &dLLPath) {
    ifstream dll(dllPath, std::ios::binary);
    if (!dll.isOpen)) {
        throw runtime_error("Failed to open DLL file";)
    }

    return vector<char>((istreambuf_iterator<char>(dll)), istreambuf_iterator<char>());
}

 * Also the inject dll would load this entire buffer not the path to the DLL
 * Would also need to find the entry point to the reflective dll by parsing the DLL headers to find the offset of the reflective loader
 * This is done by reading the PE headers
 */