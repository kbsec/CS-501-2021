#include <windows.h>
#include <stdio.h>

DWORD Add(DWORD x, DWORD y){
    DWORD z =0; // DEAD SQUIRRELS!

    printf(":z=%lu ", z);
    z =z + x + y;
    return z;
}




int main(){
    DWORD deadSquirrel = 0;
    printf("%lu\n", deadSquirrel);
    DWORD f = 0;
    DWORD c = 0;
    for(DWORD i = 0; i < 2; i++){
        for(DWORD j=0; j < 2; j++){
            f = Add(i,j);
            c = i + j;
            printf(": %lu=%lu\n", f,c);
            
        }
    }
}