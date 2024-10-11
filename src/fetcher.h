#include <Windows.h>
#include <Wininet.h>
#include <iostream>
#include <fstream>
#include <fileapi.h>
#include <string>
#include <regex>

#pragma once
#pragma comment ( lib, "Wininet.lib" )

using namespace std;

HINTERNET startSession();
void closeHandle(HINTERNET handle);
HINTERNET openUrl(HINTERNET session, LPCWSTR url);
void readBytes(HINTERNET istream, ostream& ostream );
int downloadFile(char* buff);
