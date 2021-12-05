#include <windows.h>
#include <stdio.h>
#include <intrin.h>
BOOL
PerformBaseRelocation(BYTE* lpBaseAddr,IMAGE_NT_HEADERS* ntHeaders)
{
    //unsigned char * codeBase = pLoadedModule->pCodeBase;
    PIMAGE_BASE_RELOCATION relocation;
    PIMAGE_DATA_DIRECTORY directory = &(ntHeaders)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];


    UINT_PTR delta = 	 (UINT_PTR)lpBaseAddr - (UINT_PTR)ntHeaders->OptionalHeader.ImageBase;
    wprintf(L"Offset Delta: %p\n", (void*) delta);
    if (directory->Size == 0)
    {
        return (delta == 0);
    }

    relocation = (PIMAGE_BASE_RELOCATION)(lpBaseAddr + directory->VirtualAddress);
    for (; relocation->VirtualAddress > 0; )
    {
        DWORD i;
        unsigned char * dest = lpBaseAddr + relocation->VirtualAddress;
        unsigned short * RelInfo = (unsigned short *)((unsigned char *)relocation + sizeof(IMAGE_BASE_RELOCATION));
        for (i = 0; i < ((relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2); i++, RelInfo++)
        {
            DWORD * patchAddrHL;
#ifdef _WIN64
            ULONGLONG * patchAddr64;
#endif
            INT type, offset;

            // the upper 4 bits define the type of relocation
            type = *RelInfo >> 12;
            // the lower 12 bits define the offset
            offset = *RelInfo & 0xfff;

            switch (type)
            {
            case IMAGE_REL_BASED_ABSOLUTE:
                // skip relocation
                break;

            case IMAGE_REL_BASED_HIGHLOW:
                // change complete 32 bit address
                patchAddrHL = (DWORD *)(dest + offset);
                *patchAddrHL += (DWORD)delta;
                break;

#ifdef _WIN64
            case IMAGE_REL_BASED_DIR64:
                patchAddr64 = (ULONGLONG *)(dest + offset);
                *patchAddr64 += (ULONGLONG)delta;
                break;
#endif

            default:
                break;
            }
        }

        // advance to next relocation block
        relocation = (PIMAGE_BASE_RELOCATION)(((char *)relocation) + relocation->SizeOfBlock);
    }
    return TRUE;
}

BYTE* LoadFileBytes(LPWSTR filePath, DWORD* dwSize){
    HANDLE hFile = NULL;
    wprintf(L"[*] Loading binary payload: %S\n", filePath);

    hFile = CreateFileW(
        filePath, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);

    if (!hFile) {
        wprintf(L"[!] Could not open payload: %S\n", filePath);
        return NULL;
    }
        // Note the maximum size in bytes is 2^32 
        // this is about 4 GB?
        *dwSize = GetFileSize(hFile, NULL);
        DWORD dwBytesRead = 0;
        BYTE* buffer = (BYTE*) ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, *dwSize);

        if (! ::ReadFile(hFile, buffer, *dwSize, &dwBytesRead, NULL)) {
            wprintf(L"[!] Could not read file: %d!\n", ::GetLastError());
            HeapFree(::GetProcessHeap(), 0 ,buffer);
            buffer = NULL;
        }
    
    CloseHandle(hFile);
    return buffer;
}

// Parse the Nt Headers frmom the Raw peBytes
IMAGE_NT_HEADERS* parseNtHeader(BYTE* peBytes){
     // Parse the DOS header
    IMAGE_DOS_HEADER* ntDOSHeader  = (IMAGE_DOS_HEADER*) peBytes;
    // The NT Headers begin at the offset of the PE bytes + the Address of the new headers (e_lfanew)
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*) (((UINT_PTR) ntDOSHeader) + (UINT_PTR) ntDOSHeader->e_lfanew);
    return ntHeaders;


}


// Map the PE into memory. This includes headers and sections 
void* MemoryMapPE(BYTE* peBytes, IMAGE_NT_HEADERS* ntHeaders){

    // prefered base address of the PE
    UINT_PTR preferedBaseAddress = ntHeaders->OptionalHeader.ImageBase;

    // size of PE image in bytes
    DWORD dwImageSize = ntHeaders->OptionalHeader.SizeOfImage;

    // Get the RVA of the entry poiny
    DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
    
    // size of header in bytes
    DWORD dwSizeOfHeaders = ntHeaders->OptionalHeader.SizeOfHeaders;
    wprintf(L"[+] Entrypoint RVA: %d\n",  entryPointRVA);
    wprintf(L"[+] Base address: %p\n", (void*) preferedBaseAddress);
    wprintf(L"[+] Image size:: %d\n", dwImageSize);
    wprintf(L"[+] Sizeof headers: %d\n", dwSizeOfHeaders);
    // allocate a buffer for the PE

    BYTE* lpImageBaseAddress = NULL;
    
    // try mapping image into it's prefered address 
    lpImageBaseAddress = (BYTE*) ::VirtualAlloc(
       (void*) preferedBaseAddress, 
       // NULL,
        dwImageSize, 
        MEM_RESERVE | MEM_COMMIT, 
        PAGE_EXECUTE_READWRITE
    );
    if( !lpImageBaseAddress){
        lpImageBaseAddress = (BYTE*)::VirtualAlloc(
        NULL, 
        dwImageSize, 
        MEM_RESERVE | MEM_COMMIT, 
        PAGE_EXECUTE_READWRITE 
        );
        if(!lpImageBaseAddress){
            wprintf(L"Failed to allocate virtual memory: %d\n", ::GetLastError());
            // Allocation failed 
            return NULL;
            }
    } else{
        wprintf(L"[+] Allocated memory at the prefered Base address!\n");
    }
    wprintf(L"[+] Allocated Memory at %p\n", (void*) lpImageBaseAddress);
    
    // Copy PE Headers to New base address
    memcpy(lpImageBaseAddress, peBytes, dwSizeOfHeaders);

    wprintf(L"[+] Wrote %d bytes for the headers starting at  %p and ending at %p \n", dwSizeOfHeaders, (void*) lpImageBaseAddress, (void*) (lpImageBaseAddress + dwSizeOfHeaders) );

    // Get pointer to Begininig of Image sections
    IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders); 

    // Iterate over Image Sections 
    for(int i=0; i<ntHeaders->FileHeader.NumberOfSections; ++i) {
        // Calculate VA by adding the RVA to the Base Address
        
        // Pointer to Section Bytes
        void* sBytes = (void*)((UINT_PTR)peBytes + (UINT_PTR)sections[i].PointerToRawData);
        // Where we want to write the section bytes
        void* dest = (void*) ((UINT_PTR) lpImageBaseAddress + (UINT_PTR) sections[i].VirtualAddress); 
        

        // check if there is Raw data to copy
        if(sections[i].SizeOfRawData > 0) {
            wprintf(L"Copying Section %s:%d \n", sections[i].Name, sections[i].SizeOfRawData);
            memcpy(dest,   sBytes, sections[i].SizeOfRawData);
        } else {
            // just zero it out. We might need to fill this up later. 
            memset(dest, 0, sections[i].Misc.VirtualSize);
            wprintf(L"Skipping Secion %s\n", sections[i].Name);
        }
    }
    wprintf(L"[+] Succesfully Memory Mapped PE at %p\n", (void*) lpImageBaseAddress);

    // we return a pointer to the base address of the Newly mapped PE
    return lpImageBaseAddress;
}

BOOL FixSectionPermissions(void* lpImageBase,  IMAGE_NT_HEADERS* ntHeaders){
    // Map PE sections privileges 

     // Get pointer to Begininig of Image sections
    IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeaders); 
    DWORD oldProtect = 0;
    // Headers should be Read Only
    if (!::VirtualProtect(
        lpImageBase, 
        ntHeaders->OptionalHeader.SizeOfHeaders, 
        PAGE_READONLY, 
        &oldProtect
        )){
            wprintf(L"Failed to Set Headers to Read Only because of %d\n", ::GetLastError());
            return FALSE;
        }

    //  Walk sections and fix permissions
    for(int i=0; i<ntHeaders->FileHeader.NumberOfSections; ++i) {
        // Compute Virtual Address by adding rva to the image base
        BYTE* dest = (BYTE*)lpImageBase + sections[i].VirtualAddress;

        DWORD sectionCharacteristic = sections[i].Characteristics;
        DWORD dwMemProtect = 0; //flags are not the same between virtal protect and the section header



        if(sectionCharacteristic & IMAGE_SCN_MEM_EXECUTE) {
            dwMemProtect = (sectionCharacteristic & IMAGE_SCN_MEM_WRITE) ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
                    wprintf(L"[+] Setting permission of %s to %d: Execute Read Write \n", sections[i].Name, dwMemProtect);

        } else {
            dwMemProtect = (sectionCharacteristic & IMAGE_SCN_MEM_WRITE) ? PAGE_READWRITE : PAGE_READONLY;
                    wprintf(L"[+] Setting permission of %s to %d: Readonly \n", sections[i].Name, dwMemProtect);

        }
        //wprintf(L"[+] Setting permission of %s to %d \n", sections[i].Name, dwMemProtect);
        if (!VirtualProtect(dest, sections[i].Misc.VirtualSize, dwMemProtect, &oldProtect)){
            wprintf(L"[!] Failed to change permissions: %d\n", ::GetLastError());
            return FALSE;
        }

    } 
    return TRUE;
}

// Build the Import Address table
BOOL BuildIAT(void* lpImgBaseAddr,IMAGE_NT_HEADERS* ntHeaders ){
   // lpDatadir = ntHeaders->OptionalHeader.DataDirectory;
   // Get the Import Directory Table to iterate over Required DLLs
   
   IMAGE_DATA_DIRECTORY importDataDir = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
   if (importDataDir.Size == 0){
       wprintf(L"[!] No imports!\n");
       return TRUE;
   }
   
   // Calculate pointer to Import Directory Table Object
   IMAGE_IMPORT_DESCRIPTOR* importDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)( (UINT_PTR)lpImgBaseAddr + importDataDir.VirtualAddress);

    for(int i = 0; importDescriptor[i].Name != NULL; i++){
        // Find the RVA of the start of the LPCSTR 
        char* moduleName = (char*) ((UINT_PTR)lpImgBaseAddr + importDescriptor[i].Name); 

        HMODULE hModule = ::LoadLibraryA(moduleName);
        
        // if we can't resolve our imports...just give up :D
        if(!hModule){
            wprintf(L"[!] Failed to load %s because of %d\n", moduleName, ::GetLastError());
            // panic
            return FALSE;
        }
        // the IDT has all of the libraries we need to import.
        // Now we walk import lookup table (ILT) to find all
       
        // thunk
        IMAGE_THUNK_DATA* lookupTable = (IMAGE_THUNK_DATA*) ((UINT_PTR)lpImgBaseAddr + importDescriptor[i].OriginalFirstThunk);

    if(!importDescriptor[i].OriginalFirstThunk){
            wprintf(L"[!!] No Original first thunk!!\n");
            lookupTable = (IMAGE_THUNK_DATA*) ((UINT_PTR)lpImgBaseAddr + importDescriptor[i].FirstThunk);
        }
        // the address table is a copy of the lookup table at first
        // but we put the addresses of the loaded function inside.
        // this is exactly the IAT
        // func
        IMAGE_THUNK_DATA* addressTable = (IMAGE_THUNK_DATA*) ( (UINT_PTR)lpImgBaseAddr +  importDescriptor[i].FirstThunk);

        // iterate over Lookup Table 
        for(int j=0; lookupTable[j].u1.AddressOfData != 0; ++j) {
            FARPROC* hFunction = NULL;

            // Check the lookup table for the adresse of the function name to import
             UINT_PTR lookupAddr = lookupTable[j].u1.AddressOfData;

            // the first bit here tells us whether or not we import by name or ordinal
            ////if first bit is not 1
            if(lookupAddr) { 
                // import by name : get the IMAGE_IMPORT_BY_NAME struct
         
                IMAGE_IMPORT_BY_NAME* imageImport = (IMAGE_IMPORT_BY_NAME*) ((UINT_PTR)lpImgBaseAddr + lookupAddr);
                // The null terminated function name  
                char* funcName = (char*) &(imageImport->Name);
                // The null terminated function name  
                // get that function address from it's module and name
                hFunction = (FARPROC*) GetProcAddress(hModule, funcName);
                wprintf(L"[+] Resolved %s$%s to %p\n",moduleName, funcName, hFunction);
            } else {
                // import by ordinal, directly
                hFunction = (FARPROC*) GetProcAddress(hModule, (LPSTR) lookupAddr);
                wprintf(L"[+] Resolved (ordinal) %s to %p\n", (LPSTR) lookupAddr, hFunction);
            }

            if(!hFunction) {
                wprintf(L"Failed to resolve a function in  %s because of %d\n", moduleName, ::GetLastError());
                return NULL;
            }

            // change the IAT, and put the function address inside.
            wprintf(L"[!] Setting IAT %p:%p\n" , (void*) &addressTable[j].u1.Function, (void*) hFunction);
            addressTable[j].u1.Function = (UINT_PTR) hFunction;
        }
    }
    return TRUE;
}


void HandleTLSCallbacks(void* lpImageBase,  IMAGE_NT_HEADERS* ntHeaders){
     if(ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
   {
      PIMAGE_TLS_DIRECTORY tls = (PIMAGE_TLS_DIRECTORY)( (UINT_PTR)lpImageBase + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
      PIMAGE_TLS_CALLBACK *callback = (PIMAGE_TLS_CALLBACK *) tls->AddressOfCallBacks;

      while(*callback)
      {
          wprintf(L"[+] TLS callback at %p\n", (void*) callback);
         (*callback)((LPVOID) lpImageBase, DLL_PROCESS_ATTACH, NULL);
         callback++;
      }
   } else{
       wprintf(L"No Tls Callbacks!\n");
   }
}


void* LoadPE( BYTE* peBytes ){
    // parse the Nt Header from the PE bytes
     IMAGE_NT_HEADERS* ntHeaders = parseNtHeader(peBytes);
    IMAGE_DOS_HEADER* ntDOSHeader  = (IMAGE_DOS_HEADER*) peBytes;
     // Map the headers and sections into memory 
    void* lpImageBaseAddress = MemoryMapPE(peBytes, ntHeaders);

    // Handle imports 
    // Handle Relocations 
    UINT_PTR addrDelta = 	 (UINT_PTR)lpImageBaseAddress - (UINT_PTR)ntHeaders->OptionalHeader.ImageBase;

    if(!PerformBaseRelocation((BYTE*)lpImageBaseAddress, ntHeaders  )){
        wprintf(L"NO Relocations done!\n");
    }
    BuildIAT(lpImageBaseAddress, ntHeaders );
    wprintf(L"\nALT\n");

    FixSectionPermissions(lpImageBaseAddress, ntHeaders );
    HandleTLSCallbacks(lpImageBaseAddress, ntHeaders );

    UINT_PTR lpVoidEntry =  (UINT_PTR)lpImageBaseAddress +  (UINT_PTR) ntHeaders->OptionalHeader.AddressOfEntryPoint ;

    wprintf(L"[+] New EntryPoint: %p\n",lpVoidEntry );
    return  (void*) lpVoidEntry;
}