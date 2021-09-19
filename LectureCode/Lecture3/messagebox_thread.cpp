#include <windows.h>
#include <iostream>

DWORD WINAPI  CreateMessageBox(LPVOID lpParam) {
    MessageBoxA(NULL, (char*)lpParam, "sup", MB_OK);
    std::cout << "Bye!" << std::endl;
    return 0;
}
int main(){
    auto args = "Spooky";
     HANDLE  hThreadArray[5]; 
    for (int i = 0; i< 5; i++){
        std::cout << "Starting thread" << i<< std::endl; 
    hThreadArray[i] = CreateThread(NULL, 0, &CreateMessageBox, (LPVOID)args , 0, NULL);
    }
    WaitForMultipleObjects(5, hThreadArray, TRUE, INFINITE);


}