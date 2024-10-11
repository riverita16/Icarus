#include "fetcher.h"

using namespace std;

// get a handle for internet operations
HINTERNET startSession() {
    const HINTERNET handle = InternetOpenW(0, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
    if (handle == 0) {
        const DWORD error = GetLastError();
        cerr << "[] InternetOpen(): " << error << "." << endl;
    }
    return (handle);
}

// close open handles
void closeHandle(HINTERNET handle) {
    const BOOL result = InternetCloseHandle(handle);
    if (result == FALSE) {
        const DWORD error = GetLastError();
        cerr << " [] InternetClose(): " << error << "." << endl;
    }
}

// open target remote file
HINTERNET openUrl(HINTERNET session, LPCWSTR url)
{
    const HINTERNET handle = InternetOpenUrlW(session, url, 0, 0, 0, 0);
    if (handle == 0) {
        const DWORD error = GetLastError();
        cerr << "[] InternetOpenUrl(): " << error << "." << endl;
    }
    return (handle);
}

// read remote file bytes
void readBytes(HINTERNET istream, ostream& ostream )
{
    static const DWORD SIZE = 1024;
    DWORD error = ERROR_SUCCESS;
    BYTE data[SIZE];
    DWORD size = 0;

    do {
        BOOL result = InternetReadFile(istream, data, SIZE, &size);

        if ( result == FALSE )
        {
            error = GetLastError();
            cerr << "[] InternetReadFile(): " << error << "." << endl;
        }

        ostream.write((const char*)data, size);
    }
    while ((error == ERROR_SUCCESS) && (size > 0));
}

// download the file to local drive
int downloadFile(char* url, char* output) {

    // warn user about possible file overwrite before continuing
    cout << "\n WARNING: this action will overwrite \"" << output << "\" if it already exists" << endl; 
    string res = "";

    regex ans("^ *[yYnN]{1} *$");
    do {
        cout << "Continue? (y/n) ";
        cin >> res;
    } 
    while (!regex_match(res, ans));

    if (regex_match(res, regex("^ *[nN]{1} *$"))) {
        cout << "[] Aborting file download..." << endl;
        return 0;
    } else {
        cout << "[] Proceeding with file download..." << endl;
    }

    const HINTERNET session = startSession();
    if (session != 0) {

        const HINTERNET istream = openUrl(session, (LPCWSTR) url);
        if (istream != 0) {
            
            // make sure we have an EMPTY file dest
            HANDLE hFile = CreateFileA(output, GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);          
            if (hFile == INVALID_HANDLE_VALUE) {
                cerr << "[] FAILED to open " << output << endl;

                closeHandle(istream);
                closeHandle(session);

                return 1;
            }

            CloseHandle(hFile);

            ofstream ostream(output, ios::binary);
            if (ostream.is_open()) {                
                readBytes(istream, ostream);
            }
            else {
                cerr << "[] FAILED to access " << output << endl;
            }

            closeHandle(istream);
        }
        
        closeHandle(session);
    }

    return 0;
}