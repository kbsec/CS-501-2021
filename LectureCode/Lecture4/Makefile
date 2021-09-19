CC=zig cc
CXX=zig c++
CFLAGS=-target x86_64-windows-gnu -municode
LDFLAGS= -lurlmon  -lwininet  -lshlwapi
PDB=-g
BIN=bin



all: malware evil 

malware.res:
	windres icon.rc -O coff -o $(BIN)/icon.res

malware: malware.res
	$(CXX) malware_0.cpp $(BIN)/icon.res  $(LDFLAGS) $(CFLAGS) -o $(BIN)/ClickMe.exe

evil:
	$(CXX) hacks.cpp  $(CFLAGS) -o $(BIN)/evil.exe

clean:
	rm bin/*
