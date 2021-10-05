#include <windows.h>
#include <string>
#include <iostream>
#include <winhttp.h>


std::wstring makeHttpRequest(std::wstring fqdn, int port, std::wstring uri, bool useTLS){
    std::wstring result;
    return result;
}

int wmain(int argc,  wchar_t* argv[]){
    if(argc !=5){
        std::wcout << L"Incorrect number of arguments: you need 4 positional arguemts" << std::endl;
        return 0;
    }

    std::wstring fqdn = std::wstring(argv[1]);
    int port = std::stoi( argv[2] );
    std::wstring uri = std::wstring(argv[3]);
    int  useTLS =std::stoi(argv[4]);
    bool tls;
    if (useTLS == 1){
        tls = true;
    } else if (useTLS == 0){
        tls = false;

    } else{
        std::wcout << L"bad value for useTls" << std::endl;
        return 0;
    }
     std::wcout << makeHttpRequest(fqdn,  port, uri, tls) << std::endl;
    return 0;
    
}