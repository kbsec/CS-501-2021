#include <windows.h>
#include <stdio.h>
#include "hookiat.h"


typedef void (WINAPI* hSleep)(
  DWORD dwMilliseconds
);

hSleep origSleep = Sleep;

void HookMe(){
    DWORD mSeconds = 1000 *60 * 60 * 24;
    printf("Sucks to be you, I am going to sleep for %d miliseconds\n", mSeconds);
    Sleep(mSeconds);
    printf("What? You're still here? Get a life...\n");
}



void WINAPI hookedSleep(DWORD dwMiliseconds){
        printf("[!] Hooking just saved you %d miliseconds!\n", dwMiliseconds);
        origSleep(500);  
}

int main(){
    char kernel32[] = "KERNEL32.dll";
    char sleep[] = "Sleep";
    // Get the Current processes Base Address
    void* lpImageBaseAddr = GetModuleHandleA(NULL);

    printf("Original Sleep: %p\nTarget Hook: %p\n", Sleep, hookedSleep);    
    InstallHook(lpImageBaseAddr, kernel32, sleep, (UINT_PTR)hookedSleep,(UINT_PTR*) &origSleep);
    printf("Orig Sleep: %p\n", origSleep);
    if( (UINT_PTR) Sleep == (UINT_PTR) origSleep ){
        printf("Hook Failed!\n");
        return 0;
    }
    HookMe();

    
}