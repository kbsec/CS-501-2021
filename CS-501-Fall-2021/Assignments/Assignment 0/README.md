[[Assignments]]
# Homework 0

## Due Date:  09/10/2021 by Midnight

## Submission directions 


## Definitions
- Virtual Machine 
- Guest Operating System 
- Host Operating system 

### Part 0: Setting up your environment
#### Required Reading:

https://fabian-voith.de/2020/04/21/understanding-virtualbox-networking-schemes-to-set-up-a-good-and-safe-lab/


When analyzing malware, you are strongly encouraged to run all malware in an isolated enviornment. This is to reduce the risk of accidently infecting your  host OS or other devices. Note that when running malware from an unkown threat actor, it is generally unsafe to assume that this is enough to protect yourself as there have been instances of malware leveraging bugs in hypervisors.  In particular, it is possible that the malware can detect that it is in a virtual environemnt, and  can escape either via misconfiguration or exploitation. From there,  it can spread to the host OS.  This is rare in practice but not impossible. 

This is  why when analyzing malware from a sophisticated actor, it is generlaly a good idea to do a clean install for each sample, and further more, ensure the host and guest OS are NOT connected to the internet or any critical network via any communication channel (USB, bluetooth, ...etc)

This class is going to use Virtualbox as a hypervisor, and will focus mainly on the Windows operating system, and more specifically Windows 10. 

Microsoft offers ISOs that contain  a free 90 trial of Windows 10, and comes prebuilt with the Window's Subsystem for Linux enabled, and Visual Studio. Note that if you are running this on a windows device, you unforutnelty will have to disable HyperV which break WSL2. 

This version is not a reqiurement for the class but you will need to have a windows VM.

Start by installing virtualbox.  For how to do this, see https://www.virtualbox.org/wiki/Downloads

Next, download the required ISOs
- Windows: 
	-  Opotion 1: https://developer.microsoft.com/en-us/microsoft-edge/tools/vms/ ( select MSEdge x64 )
	-  Option 2: https://developer.microsoft.com/en-us/windows/downloads/virtual-machines/  ( reccomended, but requires more space)
		-  For either option select Virtualbox 
	-  Remnux 
		-  Follow the directions here to import the Appliance: https://docs.remnux.org/install-distro/get-virtual-appliance
	- Ubuntu:	
		- You may hold off on this for now, but once we start developing C2 servers, you are advised to have a linux development environment. 
		- https://ubuntu.com/download/desktop



####  Setting up your Windows Virtua Machine 
Arch Diagram:
The goal is to put the Windows machine and remnux on an isolated network. 

![[Pasted image 20210907115127.png]]


After creating the Windows and remnux machine, configure a virtualbox internal network for both of them as follows
Click settings, and go to Network. Click on adapter 2 and enable. Then set "Attatched to" -> Internal Network 
Fix a name (I chose PrivateNetwork) and click save
![[Pasted image 20210907115427.png]]

Repeat this process for Remnux. 

**Do not run the other scripts until the configuration here is completed**


## Remnux Config
Once Remnux is booted, (the password is malware, you should probably change this :) ) , you have two tasks
- Modify the configuration for your new adapter
- Modify the configuration for a tool called `inetsim`

open up a termainl and run 
```
sudo -i
```
to drop into a root shell . 

Verify that the 3rd network itnerface is named `enp0s8` by running `ip a | grep enp0s8`
If not, look for the name of the 3rd interface and replace its  `enp0s8` with its name. 
![[Pasted image 20210907120101.png]]

From there, run the following to create a configuration file that sets your static ip on this itnerface
```bash
cat >> /etc/netplan/malware-lab.yaml << EOL
network:
        version: 2
        renderer: networkd
        ethernets:
                enp0s8:
                        dhcp4: no
                        addresses: 
                                - 10.10.10.2/24
EOL
sudo netplan apply
```
Afterwards, run `accept-all-ips start enp0s8` to allow connections  from all IPs to this machine on that itnerface.
Next, run
```
cp /etc/interim/inetsim.conf /etc/interim/inetsim.conf.bak
nano /etc/interim/inetsim.conf
```
Modify the following:
- enable the DNS server by uncommenting `start_service dns`
	- eg ![[Pasted image 20210907120637.png]]
- In service bind address,  set `service_bind_address    10.10.10.2`
	-  eg ![[Pasted image 20210907120709.png]]
- In dns_default_ip set `dns_default_ip          10.10.10.2`
- Finally, run `sudo inetsim` 


After downloading the Windows ISO,  
##### Installing Boxstarter
Follow the directions here: https://boxstarter.org/InstallBoxstarter
or run the following:
```powershell 
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://boxstarter.org/bootstrapper.ps1')); Get-Boxstarter -Force
```

You are encouraged to read the shell script or install the binary directly.

After you install Boxstarter, copy the required scripts into your VM by creating a shared folder (see https://pureinfotech.com/create-shared-folder-virtual-machine-virtualbox/ ) 
Next, open a box starter shell and run (supply the correct path to the choco script) 

You are **strongly** encouraged to read this install script. 
```powershell
$pw = Get-Credential $env:USERNAME
$choco_url = ""
Install-BoxstarterPackage -PackageName $choco_url  -Credential $pw
```


If you have ever used package managers on a flavor of Linux, the Choclaty package manager is very similar. While not directly supported by Mirosoft, it is an excelent tool that makes creating reproducable enviornments considerably easier. In the scripts folder on the website, we have a choclaty script that has all the tools you will need for this homework, and for the next few. Once they are all installed, you are encouraged to take a snapshot!


All that is left to do is to disconect the first network interface. **Do not forget to do this. If you do, malware you run will be able to connect to the internet. This will at the very least leak your IP address and at the worst could lead to other infections/damage** 

To do this, go to the bottom right of the window containing the Windows  gm. Right click on the two machines and disable the first adapter
![[Pasted image 20210907121403.png]]


Finally, verify that your machine is indeed isolated by visiting any valid website, ad observing the template page. You may also ping any ip address, and observe an incredibly small response time. Remnux is responding to all pings, serving fake DNS requests, and fake http servers among other services. You can configure these

![[Pasted image 20210907121559.png]]


# Part 1 
One of the tools downloaded in the previous setup is the Zig compiler
Zig is a new(ish) Programing language, that comes with a drop in C/C++ compiler that makes use of a technology called LLVM. 

For our purposes, it simplifies the compilation of files, and allows us to *cross compile*
This comes in handy if you wanted to, for example, create a build server, and didn't want to run Windows on it.  However, you are welcome to use gcc/g++ via mingw also installed). For this class, we will not be using the Microsoft C/C++ compiler at first. 


Required Reading:
[C++ HOME](https://www.w3schools.com/cpp/default.asp)
[C++ Intro](https://www.w3schools.com/cpp/cpp_intro.asp)
[C++ Get Started](https://www.w3schools.com/cpp/cpp_getstarted.asp)[C++ Syntax](https://www.w3schools.com/cpp/cpp_syntax.asp)
[C++ Output](https://www.w3schools.com/cpp/cpp_output.asp)
[C++ Comments](https://www.w3schools.com/cpp/cpp_comments.asp)
[C++ Variables](https://www.w3schools.com/cpp/cpp_variables.asp)
[C++ User Input](https://www.w3schools.com/cpp/cpp_user_input.asp)
[C++ Data Types](https://www.w3schools.com/cpp/cpp_data_types.asp)[C++ Operators](https://www.w3schools.com/cpp/cpp_operators.asp)
[C++ Strings](https://www.w3schools.com/cpp/cpp_strings.asp)
[C++ Math](https://www.w3schools.com/cpp/cpp_math.asp)
[C++ Booleans](https://www.w3schools.com/cpp/cpp_booleans.asp)
[C++ Conditions](https://www.w3schools.com/cpp/cpp_conditions.asp)
[C++ Switch](https://www.w3schools.com/cpp/cpp_switch.asp)
[C++ While Loop](https://www.w3schools.com/cpp/cpp_while_loop.asp)
[C++ For Loop](https://www.w3schools.com/cpp/cpp_for_loop.asp)
[C++ Break/Continue](https://www.w3schools.com/cpp/cpp_break.asp)
[C++ Arrays](https://www.w3schools.com/cpp/cpp_arrays.asp)
[C++ References](https://www.w3schools.com/cpp/cpp_references.asp)[C++ Pointers](https://www.w3schools.com/cpp/cpp_pointers.asp)


# Part 2 (Graded)
## Question 0
Consider the following C++ code 
```c++
#include <iostream>
using namespace std;

int main(int argc, char *argv[]){
	cout << "Hello world!" << endl;
}
```
Store the contents of it inside of main.cpp. To compile it, run
```
zig c++ main.cpp -o main.exe
```
Modify it to take a single argument: a name from the command line.
The program then greets the name passed as an argument. For example
```bash
$ ./main.exe Greg
Hello Greg!
```
Note that this effectively equivilant to 
```python
name = "Greg"
print(f"Hello {name}!")
```
- notice the `!` and the New line. 
- the number of command line arguments is stored in `argc`
- An array of pointers is stored in `argv`
You may only use the imports already included.

## Question 1
Read the documentation for the following function:
`https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox` (We will use MessageboxA)
Consider the following:
This is similar to the previous code snippet, but instead creates a dialogue box.
```c++
#include <windows.h>

int main(int argc, char *argv[]){
	MessageBoxA(0, "Hello world!", "I am a message box!", MB_OK);
}

```

Modify the code to allow a user to pass up to 2 arguments. the first  argument is the text data contained in the dialogue mox. the second Argument is the title observed in the dialogue box. Note that if either of these arguments are ommited,  you should use the default code above ("Hello world!"" with title "I am a message box!")

## Question 2
Complete the following code to compute the nth Fibonacci number
See https://en.wikipedia.org/wiki/Fibonacci_number for the definition of the

Fibonacci Numbers
```c++
#include <iostream>  
#include <cstdlib>  
using namespace std;

// Computes the nth fibonacci number
int Fibonacci(int n){
	
}

int main(int argc, char* argv[]){
	if(argc != 2){
	cout << "Usage: ./main.exe <int>" << endl;
	return 0;
	}
	int n = atoi(argv[1]);
	cout << Fibonacci(n) << endl;
	
}
```

Use iteration, not recursion.
In the comments, explain why when you run the above code with an input of 46, nothing happens. What is going wrong? Be specific. 


# Part 3
Required reading:
https://www.nacdl.org/Landing/ComputerFraudandAbuseAct

