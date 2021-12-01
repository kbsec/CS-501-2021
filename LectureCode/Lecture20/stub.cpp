#include <windows.h>
#include <stdio.h>
#include "peloader.h"
#include "compression.h"

#define STUB_PE_ID 101
BYTE* GetPEFromResource(DWORD* outSize){
    int ResourceID = STUB_PE_ID;
    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(ResourceID), RT_RCDATA);
            if (hResource == nullptr)
            {
                    wprintf(L"Cant Find Resource ID %d\n", ResourceID);
                    return NULL;
            }

            HGLOBAL hFileResource = LoadResource(NULL, hResource);
            if (!hFileResource)
            {
                    
                return NULL;
            }

            void* lpFile = LockResource(hFileResource);
            if (!lpFile)
            {
                    return NULL;
            }
    *outSize = SizeofResource(NULL, hResource);
    return (BYTE*) lpFile;
}

int wmain(){
    DWORD peSize = 0;
    BYTE* peBytesComp = GetPEFromResource(&peSize);
    size_t outSize = 0;
    BYTE* peBytes = DecompressData(peBytesComp, peSize,&outSize );
    VOID*  entryPoint = LoadPE(peBytes);
    DWORD dwTID = 0;
    wprintf(L"[+] Passing Execution to new EntryPoint: %p\n", entryPoint);
     HANDLE hThread = ::CreateThread(
        NULL,
        0,
       (LPTHREAD_START_ROUTINE) entryPoint,
       NULL,
       0,
       &dwTID
         );
    ::WaitForSingleObject(hThread, INFINITE);
    return 0;
}