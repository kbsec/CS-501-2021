# Assignment 2
Start this ASAP. It  
## Part 1: Coding 
### Warmup:  Base64 Encoding/Decoding 
In class, we discussed different strategies for encoding and decoding data. One of them is to base64 encode raw  bytes. In this problem, you will leverage Windows API calls to  create a simple Base64 encoder/decoder 

To accomplish this, you should make use of 
- `CryptBinaryToStringW` https://docs.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptbinarytostringw (used for encoding)
- `CryptStringToBinaryW` https://docs.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptstringtobinaryw (used for decoding )
- In both cases, you should set `dwFlags` to  `CRYPT_STRING_BASE64`

A  recreation of the desired functionality is displayed below in python 
```python 
import sys
import base64 

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} (encode|decode) data ")
        return
    data = sys.argv[2]
    if sys.argv[1] == "decode":
        print(base64.b64decode(data).decode())
    elif sys.argv[1] == "encode":
        print(base64.b64encode(data.encode()).decode())
    else:
        print("Bad args: set to either encode or decode")

if __name__ == "__main__":
    main()
		
```

```bash
$ python test_b64.py encode asdf
YXNkZg==
$ python test_b64.py decode YXNkZg==
asdf

```
### Http client 
In this section, you are going to implement a basic HTTP client using WinHTTP.  Start by reading the following documentation:
https://docs.microsoft.com/en-us/windows/win32/winhttp/winhttp-start-page

Your starter code comes with a few files:
- Makefile: Basic template for building your client 
- httpclient.cpp: main cpp program.  Contains some starter code to get you started. 


You must implement an HTTP client that takes 3 positional arguments from the command line:
- A Fully Qualified Domain Name (FQDN)
- The port  (80 for HTTP, 443 for HTTPS)
- The URI of the website 
- either 1 (use HTTPS) or 0 (Use HTTP)


It then makes an HTTP Get request  

An example script of what your code should do is recreated using python below 
```python
import sys
import requests 

def make_request(fqdn, port, uri, use_tls):
    transport = "http" if use_tls == "0" else "https"
    url = f"{transport}://{fqdn}:{port}{uri}"
    print(f"DEBUG: {url}")
    r = requests.get(url )
    if r.status_code == 200:
        print(r.text)
        return 


def main():
    if len(sys.argv) != 5:
        print(f"Example Useage: {sys.argv[0]} google.com 443 / 1")
        return 
    fqdn, port, uri, use_tls = sys.argv[1:]
    print(make_request(fqdn, port, uri, use_tls))

if __name__ == "__main__":
    main()
	
```

```bash
# example usage
# use http
python test_http.py example.com 80 / 0

# use https
python test_http.py example.com 443 / 1
```

To receive full credit, your code must  display the correct text from the response in the console using `std::wcout`.  Note that you can use Remnux to test HTTPS as well. 

You may assume that when your code is graded, that the proper command line arguments are provided. I.e., you do not *have to * do any error checking for command line arguments but it is still a good idea to do so :). 

In order to make an HTTP request, you must do the following:
- Initialize a WinHttp Session  `::WinHttpOpen()`
	```c++
	// https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpopen
	 WINHTTPAPI HINTERNET WinHttpOpen(
		  LPCWSTR pszAgentW,
		  DWORD   dwAccessType,
		  LPCWSTR pszProxyW,
		  LPCWSTR pszProxyBypassW,
		  DWORD   dwFlags
		);
	 ```
- Configure the HTTP client with `WinHttpConnect`
	```c++
	// https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpconnect
	WINHTTPAPI HINTERNET WinHttpConnect(
		  HINTERNET     hSession,
		  LPCWSTR       pswzServerName,
		  INTERNET_PORT nServerPort,
		  DWORD         dwReserved
		);
	```
- Create an HTTP Request Handle `WinHttpOpenRequest`. Hint: This is the function where you can set a flag to configure the client to use TLS 
	```c++
	//https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpopenrequest
	WINHTTPAPI HINTERNET WinHttpOpenRequest(
		  HINTERNET hConnect,
		  LPCWSTR   pwszVerb,
		  LPCWSTR   pwszObjectName,
		  LPCWSTR   pwszVersion,
		  LPCWSTR   pwszReferrer,
		  LPCWSTR   *ppwszAcceptTypes,
		  DWORD     dwFlags
		);
	```
	- For testing, you may also disable certificate verification by setting `WINHTTP_OPTION_SECURITY_FLAGS` with  `WinHttpSetoption` to ` SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE`
- Once you are happy with your request, send it via 
```c++
//hRequest is the handle to your HTTP request returend from ::WinHttpOpenRequest
//Note that in this case, we don't have any extra headers or POST data. Read the arguments for WinHttpSendRequest Carefully, as you will have to add support for sending data on the next homework :-)
::WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA,0,0,0 )
```
- Next, call `WinHttpReceiveResponse`  to get a handle to the response data. 
- Check if there is data available: `WinHttpQueryDataAvailable`
- Iterate over the response and keep calling `WinHttpReadData` until there is no more data to be read. You should accumulate all text data in a std::string buffer  buy having a fixed Char* buffer of say 4096 bytes, that you write to, and append to your buffer. 
- Cleanup all of your handles by calling `WinHttpCloseHandle` on each WinHTTP handle. 
	- You should also have sufficient error checking. I.e, if an error is encountered along the way to sending or receiving the request, you should close all open handle, and terminate the code.  To receive full credit, you must close all handles after either an error, or the HTTP request is finished. 
- Finally, return the result and print it using `std::wcout`
- Note that the grading script will use RegEx to verify your code is working. For this reason, you should not print any debugging information for the release build that you submit for grading. Handle this either by using preprocessing macros and a DEBUG  value, or simply comment out all calls to `std::wcout` aside from the final one for submission. 


## Part 2: Reverse Engineering
Ch0nky Bear is at it again. Analyze the malware.
As before, identify all relevant IOCs. Comment on how these compare to the first epoch of malware (i.e., your last assignment)

In addition, for the final payload, comment on 
1) What type of malware is this 
2) What functionality does it have
3) How does the malware uniquely identify victims?
4) What kind of system information does it collect from the victim machine?
5) How could you detect it by inspecting network/HTTP traffic. Are there any suspicious headers? 
6) What compiler was used?
7) What is the internal name of the executable?



## Part 3:  YARA Rules
Create (at most) 5 YARA Rules  to uniquely identify Ch0nky bear malware based on what you have observed thus far. You may use yarGen.py, Ghidra, or create them by hand. You may also use the hashes and pe module.  The way this will be graded, is a collection of clean binaries, random malware, and ch0nky bear will be placed in a directory. Your submitted YARA rules will be run against the binaries in the directory. For full points, you should have 0 False positives (labeled something that isn't ch0nky bear ch0nky bear) , and no False negatives (failed to label ch0nky bear malware as such).


To make your life easier, you may optionally:
1) determine what compiler was used to create this malware. 
2) Compile benign code and run yarGen.py to identify benign, compiler generated content. Add this to your "known good" yarGen database   
3) Run yarGen.py on your ch0nky and modify it as necessary. 
