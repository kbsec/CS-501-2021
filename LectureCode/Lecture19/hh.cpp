#include <windows.h>
#include <stdio.h>
int main(){
    MessageBoxA(NULL, "t", "t", MB_OK);
    wchar_t* x = L"Hello!";
    printf("%S\n", x);
    auto z = malloc(0);
    wchar_t* y = (wchar_t*) malloc(7);
    for(int i = 0; i < 7; i++){
        y[i] = x[i];
    }
    printf("%S\n", y);
}