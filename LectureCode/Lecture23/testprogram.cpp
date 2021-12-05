#include <windows.h>
#include <stdio.h>

typedef void (WINAPI* dllF)(void);
int main(){

    HMODULE hHookSleep = LoadLibraryA("hsleep.dll");
    dllF Hook = (dllF) GetProcAddress(hHookSleep, "InstallHook");
    dllF UnHook = (dllF) GetProcAddress(hHookSleep, "UnInstallHook");
    printf("Calling Hook!\n");
    Hook();
    Sleep(1000 * 3600);
    Sleep(100);
    UnHook();
    printf("[+] Unhooking! Should sleep for 2 seconds!\n");
    Sleep(2000);
    printf("Goodbye!\n");
    return 0;
   
}