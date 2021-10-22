#include <windows.h>
#include <iostream>
int wmain(int argc, WCHAR** argv){
    if( argc != 2){
        std::wcout << L"Wrong number of arguments: \nUsage: " << argv[0] <<  " path_to_lib.dll" << std::endl;
        return 0;
    }

HMODULE  lHandle = ::LoadLibraryW(argv[1]);
if (lHandle != NULL){
    std::wcout << L"Loaded " << argv[1] << std::endl;
} else{
    std::wcout << L"Failed to load " << argv[1] << std::endl;
}
Sleep(5);
::FreeLibrary(lHandle);
std::wcout << L"UnLoaded " << argv[1] << std::endl;
return 0;
}