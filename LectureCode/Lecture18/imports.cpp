#include <windows.h>
#include <stdio.h>




int wmain(){
    HMODULE hShell32 = ::LoadLibraryW(L"Shell32.dll");
    if(!hShell32){
        wprintf(L"Invalud handle for shell32: %d \n", ::GetLastError());
    }
    auto shellExecute = ::GetProcAddress(hShell32, "ShellExecuteW");
    wprintf(L"Loaded Shel32 at %p\n", (void*)hShell32 );

    HMODULE hKernel32 = ::LoadLibraryW(L"kernel32.dll");
    if (! hKernel32){
        wprintf(L"Sadness!: %d \n", ::GetLastError());
        return 0 ;
    }
    wprintf(L"Kernel32: %p\n", (void*)hKernel32);
    auto fGetTickCount = ::GetProcAddress(hKernel32, "GetTickCount");
    wprintf(L"Kernel32.dll$GetTickCount: %p\n", (void*)fGetTickCount);
    wprintf(L"The RVA of GetTickCount is %p\n", (void* ) ((UINT_PTR)fGetTickCount - (UINT_PTR) hKernel32) );
    FreeLibrary(hShell32);

    return 0;
}