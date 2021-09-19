#include <windows.h>
#include <tchar.h>


int _tmain(){
    Beep(400,1000);
    MessageBoxW(0, L"Hello world! I am the second argument!", L"I am a title and the 3rd argument", MB_ICONMASK);
}