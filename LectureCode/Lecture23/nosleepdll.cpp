#include <windows.h>
#include <stdio.h>

#include "hookiat.h"

typedef void (WINAPI* hSleep)(
  DWORD dwMilliseconds
);

hSleep origSleep = NULL;

void WINAPI sleepHandler(DWORD dwMiliseconds){
        if(dwMiliseconds > 500){
            printf("[!] Hooking just saved you %d miliseconds!\n", dwMiliseconds);
            origSleep(500);  
        } else{
            origSleep(dwMiliseconds);
        }
}

void HookSleep(){
    char kernel32[] = "KERNEL32.dll";
    char sleep[] = "Sleep";
    // Get the Current processes Base Address
    char buffer[MAX_PATH + 1];
    GetModuleFileNameA(NULL, buffer,MAX_PATH);
    printf("[!] PE: %s\n", buffer);
    void* lpImageBaseAddr = GetModuleHandleA(buffer);

    printf("Original Sleep: %p\nTarget Hook: %p\n", Sleep, sleepHandler);    
    InstallHook(lpImageBaseAddr, kernel32, sleep, (UINT_PTR)sleepHandler,(UINT_PTR*) &origSleep);
}

void UnHookSleep(){
    // Don't call this out of order or you will break stuff :D
    char kernel32[] = "KERNEL32.dll";
    char sleep[] = "Sleep";
    // Get the Current processes Base Address
    void* lpImageBaseAddr = GetModuleHandleA(NULL);

    printf("Original Sleep: %p\nTarget Hook: %p\n", Sleep, sleepHandler);    
    // Fix the hook
    InstallHook(lpImageBaseAddr, kernel32, sleep, (UINT_PTR)origSleep,(UINT_PTR*) &origSleep);
}



extern "C" __declspec(dllexport) void  WINAPI InstallHook( void){
    HookSleep();
}

extern "C" __declspec(dllexport) void  WINAPI UnInstallHook( void){
    UnHookSleep();
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{

    // Fun fact, doing anything crazy in here will BREAK EVERYTHING YOU KNOW AND LOVE
    // So like...don't :D
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
