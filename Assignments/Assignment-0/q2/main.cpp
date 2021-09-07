#include <iostream>  
#include <cstdlib>  
using namespace std;

// Computes the nth fibonacci number
int Fibonacci(int n){
	
    // changeme!
    return 0;
}

int main(int argc, char* argv[]){
	if(argc != 2){
	cout << "Usage: ./main.exe <int>" << endl;
	return 0;
	}
	int n = atoi(argv[1]);
	cout << Fibonacci(n) << endl;
	
}