CC=zig cc
CXX=zig c++
CFLAGS=-target x86_64-windows-gnu 
LDFLAGS= -lurlmon  -lwininet  -lshlwapi
PDB=-g
BIN=bin

all: encstr over_optmized

encstr:
	$(CC) main.c -o bin/encstr.exe

over_optimized:
	echo "Unsafe: strings aren't actually encrypted!"
	$(CC) main.c  -O3 -o bin/unsafe.exe