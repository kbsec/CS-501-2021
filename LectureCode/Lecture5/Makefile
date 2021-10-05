CC=zig cc
CXX=zig c++
CFLAGS=-target x86_64-windows-gnu 
LDFLAGS= -lurlmon  -lwininet  -lshlwapi
PDB=-g
BIN=bin



all: malware 

malware.res:
	windres icon.rc -O coff -o $(BIN)/icon.res

malware: malware.res
	$(CXX) malware_1.cpp $(BIN)/icon.res  $(LDFLAGS) $(CFLAGS) -o $(BIN)/ClickMe.exe -lshlwapi



clean:
	rm bin/*
