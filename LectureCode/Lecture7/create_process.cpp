#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#define BUF_SIZE 4096 

int CreateNotepad(){

    // notice that it doesn't need the .exe 
    WCHAR name[] = L"notepad";
    STARTUPINFO si  = {sizeof(si)};
    PROCESS_INFORMATION pi;
    BOOL success = ::CreateProcess(nullptr, name, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (!success){
        printf("Error creating process: %d\n", pi.dwProcessId);
        return 1;
    }
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
    return 0;
}

int ExecuteShell(){
    WCHAR cmd[] = L"C:\\Windows\\System32\\cmd.exe";
    WCHAR cmdLine[] = L"C:\\Windows\\System32\\cmd.exe /c arp -a" ;
    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    // we only need one handle for writing 
    // handle for reading, handle for writing 
    HANDLE hStdOutRd, hStdOutWr;
    // HANDLE hStdErrRd, hStdErrWr;

    //Create one-way pipe for child process STDOUT
    if (!CreatePipe(&hStdOutRd, &hStdOutWr, &sa, 0))
    {
    // error handling...
    return 1;
    }

    // Ensure that the child proecsses does not inherit the read handle
    SetHandleInformation(hStdOutRd, HANDLE_FLAG_INHERIT, 0);
    

    STARTUPINFO si = {0};
    ZeroMemory( &si, sizeof(STARTUPINFO) );

    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    // we pipe stdOut and StdErr to the same pipe
    si.hStdOutput = hStdOutWr;
    si.hStdError = hStdOutWr;

    PROCESS_INFORMATION pi = {0};
    ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
    // create the process 
    if (!CreateProcessW
        (NULL,
        cmdLine, // Command line
        NULL, // process security attributes
        NULL,  // primay thread security attributes
        TRUE,  // Handles are inerited
        CREATE_NO_WINDOW,  // creation flags
        NULL, // use parents env
        NULL,  // use parents cwd
        &si, // points to STARTUPINFO
        &pi) // recieves PROCESS_INFORMATION
     )
    {
        printf("Error Creating Process: %d\n", GetLastError());
        return 1;
        // error handling...
    }
    else
    {
        bool bProcessEnded = false;
        DWORD dwRead, dwWritten;
        CHAR chBuf[BUF_SIZE];
        BOOL bSuccess = FALSE;
        DWORD dwAvail = 0;
        for (; !bProcessEnded;){
            bProcessEnded = ::WaitForSingleObject( pi.hProcess, 50) == WAIT_OBJECT_0;
            // read from hStdOutRd and hStdErrRd as needed until the process is terminated...
            
                for (;;){
                    if (!::PeekNamedPipe(hStdOutRd, NULL, 0, NULL, &dwAvail, NULL))
                        break;

                    if (!dwAvail) // No data available, return
                        break;
                    bSuccess = ::ReadFile(hStdOutRd, chBuf, BUF_SIZE, &dwRead, NULL) ||  !dwRead;
                    if( ! bSuccess || dwRead ==0) break;
                    chBuf[dwRead] = '\0';
                    printf("%s",chBuf);

                }
        }
        ::CloseHandle(pi.hThread);
        ::CloseHandle(pi.hProcess);
    }

    ::CloseHandle(hStdOutRd);
    ::CloseHandle(hStdOutWr);
    return 0;
    }


    int _tmain(){
        ExecuteShell();
    }