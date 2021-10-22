#include <windows.h>
#include <iostream>
#include <cstdlib>


int wmain(int argc, WCHAR** argv){
    if (argc < 3){
        std::wcout << L"Usage: " << argv[0] << L" pid path_to_dll" << std::endl;
        return 0;
    }
    int pid  = _wtol(argv[1]);
    HANDLE hProcess = ::OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD,
		FALSE,  pid);
    if (!hProcess){
        std::wcout << L"Failed to open Process " << pid << L": Error " << ::GetLastError() << std::endl;
        return 0;
    }
    // allocate memory for a call to LoadLibrary()
    void* dllBuffer = ::VirtualAllocEx(
        hProcess, 
        nullptr, 
        1 << 12,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if( !dllBuffer){
        std::wcout << L"Failed to allocate memory " << ::GetLastError() << std::endl;

        return 0;
    }
    auto argBufSize =  (std::wcslen(argv[2])  + 1) * sizeof(WCHAR);
    // write small bootstrap 
    if (!::WriteProcessMemory(hProcess, dllBuffer, argv[2], argBufSize, nullptr)){
        std::wcout<< L"Failed to write process memory "<< ::GetLastError() << std::endl;
		return 0 ;
    }
    DWORD tid;
	HANDLE hThread = ::CreateRemoteThread(
        hProcess, 
        nullptr, 
        0, 
		(LPTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandleW(L"kernel32"), "LoadLibraryW"), 
		dllBuffer, // lpthread args : this is creating a callable function pointer LoadLibraryW(<MyLibrary>)
         0, 
         &tid);

    if ( !hThread){
        std::wcout << L"Failed to create remote thread! " << ::GetLastError() << std::endl;
        return 0;
    }
    
    std::wcout << L"Created thread " << tid << L" successfully!" << std::endl;

	if (WAIT_OBJECT_0 == ::WaitForSingleObject(hThread, 5000)){

		std::wcout << L"Thread Existed" << std::endl;
        }	
	
	// Cleanup
	::VirtualFreeEx(hProcess, dllBuffer, 0, MEM_RELEASE);

	::CloseHandle(hThread);
	::CloseHandle(hProcess);
    return 0;

}