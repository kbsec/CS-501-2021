CC=gcc
CXX=g++
LDFLAGS= -lcrypt32 -lbcrypt -municode
PDB=-g
BIN=bin
ifeq ($(OS),Windows_NT)
	RM=powershell /c rm
else
	RM=rm
endif
all: aes_gcm

aes_gcm:
	$(CXX) aes_test.cpp aes_gcm.cpp  $(LDFLAGS)   -o aes.exe 

clean:
	$(RM) *.exe
#g++   .\pathutils.cpp .\aes_gcm.cpp  sqlite3.o -municode -static -lshell32  -lshlwapi -lcrypt32 -lbcrypt -O3