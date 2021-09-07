# Note: if you are running this from windows, make sure to use git-bash or some other unix like shell 

zcpp = zig c++ 

q0:
	$(zcpp)  -fPIC q0/main.cpp -o bin/q0.exe

q1:
	$(zcpp)  -fPIC q1/main.cpp -o bin/q1.exe

q2:
	$(zcpp)  -fPIC q2/main.cpp -o bin/q2.exe

all : q0 q1 q2
.PHONY : all

clean: rm bin/*.exe
.PHONY: clean 