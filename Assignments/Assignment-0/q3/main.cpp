#include <iostream>
#include <string>
// Warmup 

using namespace std;


// C stile  const char pointer 
void CGreet(const char * name){
    cout << "Hello there " << name << endl;
}

// What is going wrong here?
void CPPGreet(string name){
    cout << "Hello there " << name << endl;
}


int main(){
    const char* bob = "Bob Filabuster";
    auto strBob = string(bob);
    CGreet(bob);
    CPPGreet(strBob);
    

}