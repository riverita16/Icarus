#define UNICODE
#define _UNICODE

#include <windows.h>
#pragma comment (lib, "user32.lib")

BOOL APIENTRY DllMain(HMODULE hModule, DWORD nReason, LPVOID lpReserved) {
    switch(nReason) {
        case DLL_PROCESS_ATTACH:
            MessageBox(NULL, L"WE INJECTED THIS!!", L"UF WiCyS", MB_OK);
            break;
        case DLL_PROCESS_DETACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}