CC=zig cc 
CXX=zig c++
CFLAGS=-target x86_64-windows-gnu -municode
PDB=-g
BIN=bin

all: sound crackme mbthread basicmb


sound: 
	  $(CXX) sound.cpp -lwinmm -o $(BIN)/sound.exe $(CFLAGS)

crackme:
	echo "Make sure to load the PDB into Ghidra!"
	$(CXX) classcrackme.cpp -o $(BIN)/crackme.exe $(CFLAGS) $(PDB)

mbthread:
	echo "Generating multi-threaded messagebox!"
	$(CXX) messagebox_thread.cpp -o $(BIN)/mboxthread.exe $(CFLAGS) 

basicmb:
	$(CXX) basicmb.cpp -o $(BIN)/basicmb.exe $(CFLAGS)

clean:
	echo "Cleanining Up"
	$(RM) $(PWD)/$(BIN)/*

