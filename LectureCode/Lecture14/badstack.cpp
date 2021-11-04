#include <iostream>
#include <windows.h>
#inlcude <string>

int returnFive(){
    return 5;
}

// rotates 5 character strings 
 wchar_t* RotCipher(const wchar_t* data, size_t size, const wchar_t k ){
    //const char k = 1;
    // allocate `size` characters on the heap in continguous memory 
    
    wchar_t* x = new wchar_t[size + 1];
    ::ZeroMemory(x, sizeof(wchar_t) * (size + 1));
    // it is the caller's responsiblity to free memory!
    for(size_t i=0;  i< size; i++){
        x[i] = (wchar_t) (data[i] + k);
    }
    std::wcout << L"ROT: " <<x << std::endl;

    return x;// this is incredibly dangerous!
}


int main(){
    wchar_t x[] = L"Hello";
    const wchar_t k = 1;
    size_t size = 5;
     wchar_t* result = RotCipher(x, size, k);

    std::wcout<< L"Result: " << result << std::endl;

    std::wcout << returnFive() << std::endl;

    std::wcout<< L"Result: " <<result << std::endl;
    delete[] result;
}