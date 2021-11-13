#include <windows.h>
#include <stdio.h>

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif

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

//#define _WIN64



void* ParseSections(BYTE* peBytes){
 /** Parse header **/

    IMAGE_DOS_HEADER* p_DOS_HDR  = (IMAGE_DOS_HEADER*) peBytes;
    IMAGE_NT_HEADERS* ntImageHeaders = (IMAGE_NT_HEADERS*) (((UINT_PTR) p_DOS_HDR) + (UINT_PTR) p_DOS_HDR->e_lfanew);

    // base address of the pe
    UINT_PTR hdr_image_base = ntImageHeaders->OptionalHeader.ImageBase;

    // size of exe image
    DWORD size_of_image = ntImageHeaders->OptionalHeader.SizeOfImage;
    // Get the RVA of the entry poiny
    DWORD entry_point_RVA = ntImageHeaders->OptionalHeader.AddressOfEntryPoint;
    
    DWORD size_of_headers = ntImageHeaders->OptionalHeader.SizeOfHeaders;
    wprintf(L"[+] Entrypoint RVA: %d\n",  entry_point_RVA);
    wprintf(L"[+] Base address: %p\n", (void*) hdr_image_base);
    wprintf(L"[+] Image size:: %d\n", size_of_image);
    wprintf(L"[+] Sizeof headers: %d\n", size_of_headers);
    BYTE* ImageBase =  (BYTE*)::VirtualAlloc(
        NULL, 
        size_of_image, 
        MEM_RESERVE | MEM_COMMIT, 
        PAGE_EXECUTE_READWRITE // was PAGE_READWRITE fix later 
        );
    if(!ImageBase){
        wprintf(L"Failed to allocate virtual memory: %d\n", ::GetLastError());
        return NULL;
    }
    wprintf(L"[+] Allocated Memory at %p\n", (void*) ImageBase);
 

    // Map PE sections into memory 
    memcpy(ImageBase, peBytes, size_of_headers);

    wprintf(L"[+] Wrote %d bytes for the headers\n", size_of_headers);

    // Image Section Start
    IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntImageHeaders); 

    // Iterate through ach section
    for(int i=0; i<ntImageHeaders->FileHeader.NumberOfSections; ++i) {
        // calculate the virtual Address from the RVA 
        // section[i].VirtualAddress is the RVA
        void* dest = (void*) ((UINT_PTR) ImageBase + (UINT_PTR) sections[i].VirtualAddress); 
        void* sBytes = (void*)((UINT_PTR)peBytes + (UINT_PTR)sections[i].PointerToRawData);


        if(sections[i].SizeOfRawData > 0) {
            // Copy section into memory
            wprintf(L"Copying Section %s:%d \n", sections[i].Name, sections[i].SizeOfRawData);
            memcpy(dest,   sBytes, sections[i].SizeOfRawData);
        } else {
            memset(dest, 0, sections[i].Misc.VirtualSize);
            wprintf(L"Skipping Secion %s\n", sections[i].Name);
        }
    }
    // start imports 
    IMAGE_DATA_DIRECTORY* dataDirectory = ntImageHeaders->OptionalHeader.DataDirectory;

    
    // load the address of the import descriptors table 
    IMAGE_IMPORT_DESCRIPTOR* importDescriptors = (IMAGE_IMPORT_DESCRIPTOR*) ( (UINT_PTR) ImageBase + (UINT_PTR) dataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    
    // this array is null terminated
    for(int i=0; importDescriptors[i].OriginalFirstThunk != 0; ++i) {
        
        // Get the name of the dll, and import it
        char* module_name = (char*) ImageBase + importDescriptors[i].Name;
        HMODULE import_module = LoadLibraryA(module_name);
        if(import_module == NULL) {
            return NULL;
        }

        // the lookup table points to function names or ordinals => it is the IDT
        IMAGE_THUNK_DATA* lookup_table = (IMAGE_THUNK_DATA*) (ImageBase + importDescriptors[i].OriginalFirstThunk);

        // the address table is a copy of the lookup table at first
        // but we put the addresses of the loaded function inside => that's the IAT
        IMAGE_THUNK_DATA* address_table = (IMAGE_THUNK_DATA*) (ImageBase + importDescriptors[i].FirstThunk);

        // null terminated array, again
        for(int i=0; lookup_table[i].u1.AddressOfData != 0; ++i) {
            void* function_handle = NULL;

            // Check the lookup table for the adresse of the function name to import
             UINT_PTR lookup_addr = lookup_table[i].u1.AddressOfData;

            if((lookup_addr & IMAGE_ORDINAL_FLAG) == 0) { //if first bit is not 1
                // import by name : get the IMAGE_IMPORT_BY_NAME struct
                IMAGE_IMPORT_BY_NAME* image_import = (IMAGE_IMPORT_BY_NAME*) (ImageBase + lookup_addr);
                // this struct points to the ASCII function name
                char* funct_name = (char*) &(image_import->Name);
                // get that function address from it's module and name
                function_handle = (void*) GetProcAddress(import_module, funct_name);
                wprintf(L"[+] Resolved %s$%s to %p\n",module_name, funct_name, function_handle);
            } else {
                // import by ordinal, directly
                function_handle = (void*) GetProcAddress(import_module, (LPSTR) lookup_addr);
                wprintf(L"[+] Resolved (ordinal) %s to %p\n", (LPSTR) lookup_addr, function_handle);
            }

            if(function_handle == NULL) {
                return NULL;
            }

            // change the IAT, and put the function address inside.
            address_table[i].u1.Function = (UINT_PTR) function_handle;
        }
    }

    /** Handle relocations **/

    //this is how much we shifted the ImageBase
    UINT_PTR vaRelocDelta = ((UINT_PTR) ImageBase) - (UINT_PTR) ntImageHeaders->OptionalHeader.ImageBase;
    wprintf(L"[+] Shifted Base by %p \n", (void*) vaRelocDelta);
    wprintf(L"[+] image Base Reloc VA %p\n", (void*)dataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

    wprintf(L"[+] Relocation Size: %d\n", dataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size );
    // if there is a relocation table, and we actually shitfted the ImageBase
    if(dataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress != 0 && vaRelocDelta != 0) {
        wprintf(L"Performing Relocation...\n");
        //calculate the relocation table address
        IMAGE_BASE_RELOCATION* lpReloc = (IMAGE_BASE_RELOCATION*) (ImageBase + dataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        
        //once again, a null terminated array
        while(lpReloc->VirtualAddress != 0) {

            // how any relocation in this block
            // ie the total size, minus the size of the "header", divided by 2 (those are words, so 2 bytes for each)
            UINT_PTR size = (lpReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))/2;
            // the first relocation element in the block, right after the header (using pointer arithmetic again)
            WORD* reloc = (WORD*) (lpReloc + 1);
            for(int i=0; i<size; ++i) {
                //type is the first 4 bits of the relocation word
                int type = reloc[i] >> 12;
                // offset is the last 12 bits
                int offset = reloc[i] & 0x0fff;
                //this is the address we are going to change
                DWORD* change_addr = (DWORD*) (ImageBase + lpReloc->VirtualAddress + offset);

                // there is only one type used that needs to make a change
                switch(type){
                    case IMAGE_REL_BASED_HIGHLOW :
                        *change_addr += vaRelocDelta;
                        break;
                    default:
                        break;
                }
            }

            // switch to the next relocation block, based on the size
            lpReloc = (IMAGE_BASE_RELOCATION*) (((UINT_PTR) lpReloc) + lpReloc->SizeOfBlock);
        }
    }
    else{
        wprintf(L"[!] No Relcations performed!\n");
    }
//end

    /** Map PE sections privileges **/

    //Set permission for the PE header to read only
    DWORD oldProtect;
    VirtualProtect(ImageBase, ntImageHeaders->OptionalHeader.SizeOfHeaders, PAGE_READONLY, &oldProtect);

    for(int i=0; i<ntImageHeaders->FileHeader.NumberOfSections; ++i) {
        BYTE* dest = (BYTE*)ImageBase + sections[i].VirtualAddress;
        DWORD s_perm = sections[i].Characteristics;
        DWORD v_perm = 0; //flags are not the same between virtal protect and the section header
        if(s_perm & IMAGE_SCN_MEM_EXECUTE) {
            v_perm = (s_perm & IMAGE_SCN_MEM_WRITE) ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
        } else {
            v_perm = (s_perm & IMAGE_SCN_MEM_WRITE) ? PAGE_READWRITE : PAGE_READONLY;
        }
        wprintf(L"[+] Setting permission of %s to %d \n", sections[i].Name, v_perm);
        if (!VirtualProtect(dest, sections[i].Misc.VirtualSize, v_perm, &oldProtect)){
            wprintf(L"[!] Failed to change permissions: %d\n", ::GetLastError());
        }

    } 

    wprintf(L"[+] Base and offset: %p %p\n", (void*)ImageBase, (void*) ntImageHeaders->OptionalHeader.AddressOfEntryPoint);
  return  (void*) (ImageBase + ntImageHeaders->OptionalHeader.AddressOfEntryPoint);
}

void* LoadPE( BYTE* PE ){

    void* lpVoidEntry = ParseSections(PE);
    wprintf(L"[+] New EntryPoint: %p\n",lpVoidEntry );
    return lpVoidEntry;
}

int wmain(int argc, wchar_t* argv[]){
    if (argc != 2){
        wprintf(L"Usage: %S <path_to_pe>\n", argv[0]);
        return 0;
    }
    //MessageBoxA(NULL, "a", "a", MB_OK);
    DWORD dwFileSize = 0;
    BYTE* fileBytes = LoadFileBytes(argv[1], &dwFileSize);
    if (!fileBytes){
        return 0;
    } 
    void* address =LoadPE(fileBytes);
    ::HeapFree(::GetProcessHeap(), 0, fileBytes);

    return 0;

}