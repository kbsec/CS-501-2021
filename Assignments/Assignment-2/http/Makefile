CC=zig cc
CXX=g++
CFLAGS=-target x86_64-windows-gnu 
LDFLAGS= -lwinhttp  -municode
PDB=-g
BIN=bin

all: base64

base64:
	$(CXX) httpClient.cpp  $(LDFLAGS)   -o http.exe 

clean:
	rm *.exe