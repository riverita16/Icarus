#include "dll_inject.h"
#include <iostream>

using namespace std;

int main() {
    DLLInjector injector(L"Notepad.exe", "C:\\Users\\Alanis\\Documents\\CS\\Icarus\\testing\\test.dll");
    if (injector.inject()) {
        cout << "Successful DLL injection" << endl;
    } else {
        cerr << "DLL injection failed" << endl;
    }

    return 0;
}