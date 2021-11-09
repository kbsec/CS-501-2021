# Assignment 3

## Reversing
Ch0nky is at it again, but this time, it would seem they have used the exact same maldoc from a previous campaign. Is anything differing? 

In order for the code to run, you must have the correct visual C and C++ runtime DLLs . The VMs are setup to have all dependencies required, but if you are missing any please see this question 
https://answers.microsoft.com/en-us/windows/forum/all/msvcp140dll-is-missing-in-my-win-10/1c65d6b0-68b8-4b59-b720-3e6a33038389?auth=1

### stage 0
Maldoc: identical. Nothing to be done here

### Stage 1
Dropper: How does this dropper differ from previous Epochs

### Payload:
Enumerate the functionality for 
1) defense evasion
2) Execution
3) Persistence (does it have a mechanism to stay on the machine?)
4) What commands does it pull down from the server?
5) Is there a final stage payload that we haven't observed yet?


### Loaded Payload
The primary difference between this epoch and the previous one i 

## Coding 
### Chromium Stealer 
In this portion of the assignment,  you will be implementing a stealer targeting Chrome based browsers. This code should work on Chromium, Google Chrome, Brave, and Microsoft Edge. However, to keep things simple, you are only responsible for parsing  passwords and cookies from Chrome. 
For passwords, you should retrieve the URL (not the action one), the Username, and the password. 
For cookies, you should obtain the site, and the cookie value
You may use whatever output format you want, but make sure there are at the very least spaces between the values, as the grading code will be matching regex. You may output the results in UTF-8 (think `printf` or `wprintf("%s")`)

You are welcome to use any SQLite3 client you want, but it must be something you can compile, and ship with your executable. I recommend the C SQLite3 amalgamation 
https://www.sqlite.org/amalgamation.html

To call it from C++, you must create an object file, and link your binary against it. I.e.
```
g++ -c sqlite3.c 
# creates sqlite3.o
# compile with object file 
g++ mycode.cpp sqlite3.o  -static 
```



As covered in class, the steps for retrieving passwords are as follows:
1) Execute a process on the victim machine that has the correct access rights (you may assume this is done)
2)  Copy the contents of the Sqlite database into a temporary working directory. Remember, SQLIte supports only a single client, and if the browser is using it, the DB will be locked! 
3)  Parse the the DPAPI encrypted symmetric key from  json file. You do not need a json library for this.
4)  Decrypt the symmetric key using the DPAPI 
5)  Use the  key to decrypt the fields stored for user's History, Cookies, Passwords, and autofill data. Each of those can be retrieved by executing an SQL query against the chrome DB, retrieving  the relevant values, and decrypting them. Note that the values are encrypted with AES-GCM, mode.
	1)  Refer to the python script from lecture, but recall that the first 3 bytes correspond to the chrome version, and should b e omitted. The next 23 bytes are the IV, and the final 16 bytes are the TAG/Message Authentication Code (referred to as a TAG by windows)
6)  For simplicity, you may assume each user only has a single profile, contained in the default folder. No need to walk directories

Note this is not comprehensive: look at the python script and identify encoding/decoding required.  

For memory management, you may use 
- malloc/free
- HeapAlloc/HeapFree
- new/delete
- Vectors/Strings  as containers
You may not use LocalAlloc or VirtualAlloc.

#### Part 1
- Write a Client that interacts with the Windows Crypto API to Write an AES-GCM cryptography client
- This should support Encryption and Decryption . See the starter code and makefile for more.

#### Part 2
- Write a Lightweight file parser that grabs the contents of a file between two tokens. Test this out on the encrypted Chrome key. Note that Chrome UTF-8 encodes its files.
- It might help to implement some basic file utilities  

#### Part 3
-  Create an executable that when run,  prints out cracked chrome credentials

You may assume this version of chrome encrypts the symmetric key with the DPAPI, and the cipher used to encrypt credentials is AES-GCM
For full credit, you need to retrieve all credentials for the default user profile including 
1) Login information: username, password, url
2) Cookies: Cookie value, url 

To test this out, add some fake credentials to your chrome browser on the VM.


### Adversary Emulation
You may work in groups of up to 4 people for this section. 

Adversary Emulation: reverse the contents of Ch0nky Shell and implement its 3 core features 

#### HTTP Client 
- Look at the way that the malware 
- 1) checks in with the server
- 2) Pulls down tasks
- 3) Performs defense evasion (proxy flags, exit conditions...etc)
- 4) Authentication (headers, passwords...etc)

#### Powershell
Implement Ch0nkyShell's Powershell execution functionality. I.e., without writing anything to disk, use `CreateProcess` with std out and std err redirected to a named pipe. Note that it captures the output without writing anything to disk.

#### RunKey Persistence 
Google "Run Key Persistence"
If you missed this feature from the previous section, fear not, you get to figure out what what it does, its purpose, and how to implement it yourself :-)

To test your code, you are welcome to create a simple clone of the ch0nky C2 or use the live one located at `ch0nky[.]chickenkiller.com`

This should be a drop in replacement for the ch0nky shell observed on homework 2. 

#### Commands
Observe the commands pulled down by the agent from the previous homework and this one. What commands do they execute? What do those commands do? Explain what the purpose of the shell might be. Is there a final stage payload? 
