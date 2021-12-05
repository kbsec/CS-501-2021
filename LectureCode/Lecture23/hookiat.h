#include <windows.h>

void InstallHook(void* lpImageBaseAddr, char* library,  char* funcToHook,  UINT_PTR lpHookAddr, UINT_PTR *saveAddr );