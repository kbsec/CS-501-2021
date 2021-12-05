#include <stdio.h>
#include <windows.h>


bool StrEq(char* str0, char* str1){
    int i;
    for(i = 0; str0[i] != '\0'; i++){
        if (str0[i] != str1[i]){
            return false;
        }
        if (i >= MAX_PATH){
            return false;
        }
    }
    return str0[i] == str1[i]; 
}


void InstallHook(void* lpImageBaseAddr, char* library,  char* funcToHook,  UINT_PTR lpHookAddr, UINT_PTR *saveAddr ){
    // Step one, Parse the DOS Header 
    IMAGE_DOS_HEADER* lpDosHeader = (PIMAGE_DOS_HEADER) lpImageBaseAddr;
    
    // Step two: Find the offset of the NtHeader
    PIMAGE_NT_HEADERS lpNtHeaders = (PIMAGE_NT_HEADERS) ((UINT_PTR )lpImageBaseAddr + lpDosHeader->e_lfanew );
    
    // Step three: Walk the directory Export table
    IMAGE_DATA_DIRECTORY  imageDataDir = (IMAGE_DATA_DIRECTORY) lpNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    PIMAGE_IMPORT_DESCRIPTOR imgImportDescr = (PIMAGE_IMPORT_DESCRIPTOR) ((UINT_PTR) lpImageBaseAddr + imageDataDir.VirtualAddress);
    for(int i = 0; imgImportDescr[i].Name != NULL; i++  ){
        char* libName = (char*) ((UINT_PTR) lpImageBaseAddr + imgImportDescr[i].Name);
        // check if the two libs are the same
        if(!StrEq( libName, library)){
            continue;
        }
        printf("[+] Found Library %s!\n", library);
        // Walk import address table and import descriptor table
        PIMAGE_THUNK_DATA lpLookupTable = (PIMAGE_THUNK_DATA) ((UINT_PTR)lpImageBaseAddr + imgImportDescr[i].OriginalFirstThunk);
        if(!lpLookupTable){
             lpLookupTable = (IMAGE_THUNK_DATA*) ((UINT_PTR)lpImageBaseAddr + imgImportDescr[i].FirstThunk);
        }   
        PIMAGE_THUNK_DATA lpAddrTable = (PIMAGE_THUNK_DATA) ((UINT_PTR)lpImageBaseAddr + imgImportDescr[i].FirstThunk);

        // Walk the Descriptor and Lookup table in paralel
        for(int j = 0; lpLookupTable[j].u1.AddressOfData != 0; ++j){
             UINT_PTR lpLookupAddr = lpLookupTable[j].u1.AddressOfData;
            IMAGE_IMPORT_BY_NAME* lpImgImport = (IMAGE_IMPORT_BY_NAME*) ((UINT_PTR) lpImageBaseAddr + lpLookupAddr);
            char* funcName = (char*) &lpImgImport->Name;
            if(StrEq(funcToHook,funcName)){
                    printf("[+] Found %s$%s\n", library, funcToHook);
                    // time toe Hook the function
                    DWORD oldProtect = 0;

                    if(!VirtualProtect((LPVOID)(&lpAddrTable[j].u1.Function), sizeof(UINT_PTR), PAGE_READWRITE, &oldProtect)){
                        printf("[!] Failed to set memory protections: %d\n", ::GetLastError());
                    }
						
					// swap MessageBoxA address with address of hookedMessageBox
                    printf("[+] Hooked %s$%s: %p -> %p\n", library, funcToHook, lpAddrTable[j].u1.Function, lpHookAddr);
                    UINT_PTR mbLoc = (UINT_PTR)lpAddrTable[j].u1.Function;
                    // Set the save address (the real adddress of the hook)
                    *saveAddr =  mbLoc;
                    printf("[+] Saved function Addr: %p\n", *saveAddr);
					lpAddrTable[j].u1.Function = lpHookAddr;
                    if(!VirtualProtect((LPVOID)(&lpAddrTable[j].u1.Function), sizeof(UINT_PTR), oldProtect, &oldProtect)){
                        printf("[!] Failed to set memory protections: %d\n", ::GetLastError());

                    }
                    printf("[+] New address of Function: %p\n", lpAddrTable[j].u1.Function);
                    return;
                } 

        }


    }
    printf("[!] Warnining: no function was hooked!\n");
}
