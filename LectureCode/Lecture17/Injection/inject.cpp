#include <windows.h>
#include <iostream>
#include <cstdlib>


// note that for 64bit machine, an address type is unsigned 64 bit integer
// For 32bit, it would be unsigned 32  bit
long long CalculateExportedFuncAddr( LPCWSTR lpPath, HMODULE hPayloadBase, LPCSTR lpFunctionName ) {
  // Load payload in our own virtual address space
  HMODULE hLoaded = LoadLibraryW( lpPath );

  if( hLoaded == NULL ) {
    return 0;
  } else {
      // Calculate the offset 
    auto  lpFunc   = GetProcAddress( hLoaded, lpFunctionName );
    long long dwOffset = (long long)lpFunc - (long long)hLoaded;
    std::wcout << L"Base addr: " << hPayloadBase << L" func Addr: " << (long long)lpFunc << L" Offset: " << dwOffset<<std::endl;
    FreeLibrary( hLoaded );
    return (long long )hPayloadBase + (long long) dwOffset;
  }
}




int wmain(int argc, WCHAR** argv){
    if (argc !=  3){
        std::wcout << L"Usage: " << argv[0] << L" pid path_to_dll exportedFunc" << std::endl;
        return 0;
    }
    DWORD pid  = _wtol(argv[1]);
    // exported function name
    char funcName[] = "Surprise";

    HMODULE hInjected= NULL;
    // get handle to process we wish to inject in
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
		dllBuffer, // Force the remote function to call Load Library <myModule>
         0, 
         &tid);

    if ( !hThread){
        std::wcout << L"Failed to create remote thread! " << ::GetLastError() << std::endl;
        return 0;
    }
    
    std::wcout << L"Created thread " << tid << L" successfully!" << std::endl;

	if (WAIT_OBJECT_0 == ::WaitForSingleObject(hThread, 5000)){

		std::wcout << L"Thread Exited! Time to inject " << std::endl;
        // Get the base Address of the proc in the remote thread
        // this gives us the base address because our thread returns a handle to the module!
        // this is a hack, and a safer way to do this would be to enumerate loaded processes
        // but this is fine for our purposes 
        ::GetExitCodeThread( hThread, ( LPDWORD )&hInjected );

        long long addr = CalculateExportedFuncAddr(argv[2], hInjected, funcName);
     HANDLE hThread = CreateRemoteThread( hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)addr, NULL, 0, NULL );

       

        }	
	
	// Cleanup
	::VirtualFreeEx(hProcess, dllBuffer, 0, MEM_RELEASE);
    if(hThread){
        	::CloseHandle(hThread);

    }
    if( hProcess){
	::CloseHandle(hProcess);

    }
    return 0;

}