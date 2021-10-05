#include <windows.h>
#include <tchar.h>
#include <fileapi.h>
#include <shlwapi.h>

// Example downloader. 
#define CHONKY_PATH "C:\\malware\\ch0nky.txt"


void ChonkyCheck(){
    LPCSTR szPath = CHONKY_PATH;
    DWORD dwAttrib = GetFileAttributesA(szPath);
    BOOL result = (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    if (!result){
        // not Ch0nky
        exit(0);
    }

}


int _tmain(){
    if (! ChonkyCheck){
        exit(0);
    }

}