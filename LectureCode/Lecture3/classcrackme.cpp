#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <iostream>


void CrackMe(int argc, _TCHAR* argv[]){
    int sum = 0;
    for(int i=1; i< argc; i++){
        sum += _tstoi(argv[i]);
    }

    if (sum << 1 == 84){
        std::cout << "Cracked!" << std::endl;
    } else{
        std::cout << "Failure!" << std::endl;
    }
    
}


int _tmain(int argc,   _TCHAR* argv[]){
    CrackMe(argc, argv );
    return 0;
}