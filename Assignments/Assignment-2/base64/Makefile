CC=zig cc
CXX=g++
CFLAGS=-target x86_64-windows-gnu 
LDFLAGS= -lCrypt32  -municode
PDB=-g
BIN=bin

all: base64

base64:
	$(CXX) base64.cpp  $(LDFLAGS)   -o base64.exe 

clean:
	rm *.exe