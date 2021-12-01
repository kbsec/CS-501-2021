#include <windows.h>
#include <stdio.h>
#include "compression.h"

BOOL WriteByteToFile(LPWSTR filePath, BYTE* data,DWORD filesize){
    HANDLE hFile = NULL;
    BOOL bErrorFlag = FALSE;
    DWORD dwBytesWritten = 0;
    hFile = ::CreateFileW(
        filePath, 
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_NEW,             // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);

    if (!hFile) {
        wprintf(L"[!] Could not open payload: %S\n", filePath);
        return NULL;
    }
      bErrorFlag = ::WriteFile( 
                    hFile,           // open file handle
                    data,      // start of data to write
                    filesize,  // number of bytes to write
                    &dwBytesWritten, // number of bytes that were written
                    NULL);            // no overlapped structure

    if (FALSE == bErrorFlag)
    {
       wprintf(L"[!] Unable to write to file %d\n", ::GetLastError());
        ::CloseHandle(hFile);

       return FALSE;
    }
    else
    {
        if (dwBytesWritten != filesize)
        {
            // This is an error because a synchronous write that results in
            // success (WriteFile returns TRUE) should write all data as
            // requested. This would not necessarily be the case for
            // asynchronous writes.
            wprintf(L"[!] Wrote the wrong number of bytes!\n");
            ::CloseHandle(hFile);

            return FALSE;
        }
        else
        {
            wprintf(L"[+] Wrote %d bytes to %s successfully.\n", dwBytesWritten, filePath);
        }
    }

    ::CloseHandle(hFile);
    return TRUE;
}

BYTE* LoadFileBytes(LPWSTR filePath, DWORD* dwSize){
    HANDLE hFile = NULL;
    wprintf(L"[*] Loading binary payload: %S\n", filePath);

    hFile = CreateFileW(
        filePath, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);

    if (!hFile) {
        wprintf(L"[!] Could not open payload: %S\n", filePath);
        return NULL;
    }
        // Note the maximum size in bytes is 2^32 
        // this is about 4 GB?
        *dwSize = GetFileSize(hFile, NULL);
        DWORD dwBytesRead = 0;
        BYTE* buffer = (BYTE*) ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, *dwSize);

        if (! ::ReadFile(hFile, buffer, *dwSize, &dwBytesRead, NULL)) {
            wprintf(L"[!] Could not read file: %d!\n", ::GetLastError());
            HeapFree(::GetProcessHeap(), 0 ,buffer);
            buffer = NULL;
        }
    
    CloseHandle(hFile);
    return buffer;
}

int wmain(int argc, WCHAR* argv[]){
     if (argc != 3){
        wprintf(L"Usage: %S <path_to_pe> <outfile> \n", argv[0]);
        return 0;
    }
    DWORD dwFileSize = 0;
    BYTE* fileBytes = LoadFileBytes(argv[1], &dwFileSize);
    wprintf(L"[+] File size: %d bytes\n", dwFileSize);
    if (!fileBytes){
        return 0;
    } 
    size_t outSize = 0;
    BYTE* compressedData = CompressData(fileBytes, dwFileSize, &outSize);
    BOOL res = WriteByteToFile(argv[2], compressedData, outSize);
    if (!res){
        wprintf(L"[!] Failed to create dump file!\n");
    } else{
        wprintf(L"[+] Success!\n");
    }
    return 0;
}
