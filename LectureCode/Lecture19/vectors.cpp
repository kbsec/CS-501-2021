#include <windows.h>
#include <vector>
#include <stdio.h>
using namespace std;

int main(){
    MessageBoxA(NULL, "", "", MB_OK);
    vector<char> x;
    x.push_back((char) 90);
    printf("%p\n" ,(void*) &x);

    printf("%p\n" ,(void*) &x[0]);
    printf("%c\n", (char) x[0]);
}
