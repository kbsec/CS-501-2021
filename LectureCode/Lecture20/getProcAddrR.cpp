#include <windows.h>

// https://github.com/stephenfewer/ReflectiveDLLInjection/blob/master/inject/src/GetProcAddressR.c
FARPROC WINAPI GetProcAddressR( HANDLE hModule, LPCSTR lpProcName )
{
	UINT_PTR uiLibraryAddress = 0;
	FARPROC fpResult          = NULL;

	if( hModule == NULL )
		return NULL;

	// a module handle is really its base address
	uiLibraryAddress = (UINT_PTR)hModule;

	__try
	{
		UINT_PTR uiAddressArray = 0;
		UINT_PTR uiNameArray    = 0;
		UINT_PTR uiNameOrdinals = 0;
		PIMAGE_NT_HEADERS pNtHeaders             = NULL;
		PIMAGE_DATA_DIRECTORY pDataDirectory     = NULL;
		PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
			
		// get the VA of the modules NT Header
		pNtHeaders = (PIMAGE_NT_HEADERS)(uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew);

		pDataDirectory = (PIMAGE_DATA_DIRECTORY)&pNtHeaders->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

		// get the VA of the export directory
		pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)( uiLibraryAddress + pDataDirectory->VirtualAddress );
			
		// get the VA for the array of addresses
		uiAddressArray = ( uiLibraryAddress + pExportDirectory->AddressOfFunctions );

		// get the VA for the array of name pointers
		uiNameArray = ( uiLibraryAddress + pExportDirectory->AddressOfNames );
				
		// get the VA for the array of name ordinals
		uiNameOrdinals = ( uiLibraryAddress + pExportDirectory->AddressOfNameOrdinals );

		// test if we are importing by name or by ordinal...
		if( ((DWORD)lpProcName & 0xFFFF0000 ) == 0x00000000 )
		{
			// import by ordinal...

			// use the import ordinal (- export ordinal base) as an index into the array of addresses
			uiAddressArray += ( ( IMAGE_ORDINAL( (DWORD)lpProcName ) - pExportDirectory->Base ) * sizeof(DWORD) );

			// resolve the address for this imported function
			fpResult = (FARPROC)( uiLibraryAddress + DEREF_32(uiAddressArray) );
		}
		else
		{
			// import by name...
			DWORD dwCounter = pExportDirectory->NumberOfNames;
			while( dwCounter-- )
			{
				char * cpExportedFunctionName = (char *)(uiLibraryAddress + DEREF_32( uiNameArray ));
				
				// test if we have a match...
				if( strcmp( cpExportedFunctionName, lpProcName ) == 0 )
				{
					// use the functions name ordinal as an index into the array of name pointers
					uiAddressArray += ( DEREF_16( uiNameOrdinals ) * sizeof(DWORD) );
					
					// calculate the virtual address for the function
					fpResult = (FARPROC)(uiLibraryAddress + DEREF_32( uiAddressArray ));
					
					// finish...
					break;
				}
						
				// get the next exported function name
				uiNameArray += sizeof(DWORD);

				// get the next exported function name ordinal
				uiNameOrdinals += sizeof(WORD);
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		fpResult = NULL;
	}

	return fpResult;
}