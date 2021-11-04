#include <windows.h>
#include <iostream>


#define MEM_SIZE 1000000

DWORDLONG printMemorUsed()
{
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof (statex);
  GlobalMemoryStatusEx (&statex);
   DWORDLONG virtualMemUsed = statex.ullTotalPageFile - statex.ullAvailPageFile;

  std::cout << "There is " << virtualMemUsed << " bytes of memory in use!" << std::endl;
  return virtualMemUsed;
}

char* LeakyBoi(){
    
    char* x =  new char[MEM_SIZE]; // this never gets freed!!
    x[MEM_SIZE - 1] = 0; 
    return x;
}
int main(){
        char *x;
        auto startMem = printMemorUsed();
        for(int i=0; i< 1000; i++){
            x = LeakyBoi();
            // do some work
           // delete [] x; // 1007706112 - 30183424
        }
        
        Sleep(5000);
        std::cout << printMemorUsed() - startMem << " bytes of memory added" << std::endl;// we will eventuall run out of memory
}