#include <windows.h>
#include <stdio.h>
#include "hookiat.h"

typedef int (WINAPI* _MessageBoxA)(
  HWND   hWnd,
  LPCSTR lpText,
  LPCSTR lpCaption,
  UINT   uType
);

 _MessageBoxA origMessageBoxA = MessageBoxA;


 int WINAPI hookedMessageBoxA(
  HWND   hWnd,
  LPCSTR lpText,
  LPCSTR lpCaption,
  UINT   uType
){
    printf("[+] Hooked function Arguments: %s, %s, %d\n", lpText, lpCaption, uType);
    origMessageBoxA(hWnd, "THE HOOKS, THE HOOKS!", "GetHooked son!", uType);
    return 0;
}



void HookMe(){
    MessageBoxA(NULL, "Howdy Mr Krabs! I am not hooked!", "Booo!", MB_OK);
}






int main(){
    char user32[] = "USER32.dll";
    char mbox[] = "MessageBoxA";
    // Get the Current processes Base Address
    void* lpImageBaseAddr = GetModuleHandleA(NULL);
    printf("Original MessageBoxA: %p\n", (void*)MessageBoxA);
    HookMe();
    
    InstallHook(lpImageBaseAddr, user32, mbox, (UINT_PTR)hookedMessageBoxA,(UINT_PTR*) &origMessageBoxA);
    printf("Hooked MessageBoxA: %p\n", (void*)MessageBoxA);

    HookMe();    
    InstallHook(lpImageBaseAddr, user32, mbox, (UINT_PTR)origMessageBoxA,(UINT_PTR*) &origMessageBoxA);
    printf("Unhooked MessageBoxA: %p\n", (void*)MessageBoxA);

    HookMe();

}