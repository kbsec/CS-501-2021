#include <windows.h>

int wmain(){
    ::MessageBoxW(NULL, L"Hello!", L"world!", MB_OK);
    return 0;
}