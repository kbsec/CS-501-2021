CC=gcc 
CXX=g++
CFLAGS=-municode
DLL=-shared



all: testDLL loadlib inject 

testDLL:
	$(CXX) testdll.cpp $(CFLAGS) $(DLL) -o test.dll

loadlib:
	$(CXX) load_dll.cpp $(CFLAGS) -o loaddll.exe

inject:
	$(CXX) injectdll.cpp $(CFLAGS) -o inject.exe

clean:
	powershell.exe /c rm *.exe 
	powershell.exe /c rm *.dll