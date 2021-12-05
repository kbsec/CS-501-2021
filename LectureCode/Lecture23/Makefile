CC=gcc 
CXX=g++
CFLAGS=-municode
LDFLAGS=-lWinmm
DLL=-shared

ifeq ($(OS),Windows_NT)
	RM=powershell /c rm
else
	RM=rm
endif


all: hookmb hooksleep hookdll testProg

hookmb:
	$(CXX) hookiat.cpp hookmb.cpp -o hookmb.exe

hooksleep:
	$(CXX) hookiat.cpp hook_sleep.cpp -o hook_sleep.exe

hookdll:
	$(CXX) hookiat.cpp nosleepdll.cpp $(DLL) -o hsleep.dll

testProg:
	$(CXX) testprogram.cpp -o test.exe

clean:
	$(RM) *.exe
	$(RM) *.dll