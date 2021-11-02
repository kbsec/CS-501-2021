
CXX=g++
CFLAGS=-Wall

all: deads leak stack

leak:
	$(CXX) leakyboi.cpp $(CFLAGS) -o leak.exe 
deads:
	$(CXX) dead_squirrels.cpp $(CFLAGS) -o dead_squirrels.exe 
stack:
	$(CXX) badstack.cpp $(CFLAGS) -o badstack.exe

clean:
	rm *.exe