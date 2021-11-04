#include <windows.h>
#include <stdio.h>
#include <bcrypt.h>



#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)

#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001L)

//auto validHashes  = {BCRYPT_SHA1_ALGORITHM, BCRYPT_SHA256_ALGORITHM,BCRYPT_SHA384_ALGORITHM,BCRYPT_SHA512_ALGORITHM,BCRYPT_MD2_ALGORITHM, BCRYPT_MD4_ALGORITHM,BCRYPT_MD5_ALGORITHM };


class WinHash{

    BCRYPT_ALG_HANDLE hAlg;
    BCRYPT_HASH_HANDLE hHash;
    //LPCWSTR pszAlgId;
    NTSTATUS status;
    bool err; 
    DWORD cbData ;
  
    DWORD cbHashObject;
    BYTE* pbHashObject;
 
    

    void Cleanup();

    public:
        DWORD  cbHash;
        BYTE*    pbHash;
        BOOL Update(BYTE* data, size_t dataLen);
        BOOL Digest();
        WinHash(LPCWSTR pszAlgId);
        ~WinHash();
};

void WinHash::Cleanup(){
                 wprintf(L"Cleanup Called\n");

      if(hAlg)
        {
            ::BCryptCloseAlgorithmProvider(hAlg,0);
             wprintf(L"Cleaned up AlgorithmProvider\n");
             hAlg = NULL;
        }

        if (hHash)    
        {
            ::BCryptDestroyHash(hHash);
             wprintf(L"Cleaned up Hash Object\n");
             hHash = NULL;
        }

        if(pbHashObject)
        {
            ::HeapFree(GetProcessHeap(), 0, pbHashObject);
             wprintf(L"Cleaned up Heap SPace for Hash Object\n");
             pbHashObject = NULL;
        }

        if( (pbHash) && err )
        {
            ::HeapFree(GetProcessHeap(), 0, pbHash);
             wprintf(L"Cleaned up Hash space\n");
                pbHash = NULL;
        }

}

static const BYTE rgbMsg[] = 
{
    0x61, 0x62, 0x63
};

WinHash::WinHash(LPCWSTR pszAlgId){
    // initialize  required vars 
    hAlg            = NULL;
    hHash           = NULL;
    status          = STATUS_UNSUCCESSFUL;
    cbData          = 0,
    cbHash          = 0,
    cbHashObject    = 0;
    pbHashObject    = NULL;
    pbHash          = NULL;
    err = false;
      //open an algorithm handle
    if(!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                                                &hAlg,
                                                pszAlgId,
                                                NULL,
                                                0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
        Cleanup();
        return;
    }
  //calculate the size of the buffer to hold the hash object
    if(!NT_SUCCESS(status = BCryptGetProperty(
                                        hAlg, 
                                        BCRYPT_OBJECT_LENGTH, 
                                        (PBYTE)&cbHashObject, 
                                        sizeof(DWORD), 
                                        &cbData, 
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
         Cleanup();
         return;
    }

    //allocate the hash object on the heap
    // PBYTE <=> BYTE* <=> * uint8_t <=> *unsinsinged char 
    pbHashObject = (PBYTE)HeapAlloc (GetProcessHeap (), 0, cbHashObject);
    if(NULL == pbHashObject)
    {
        wprintf(L"**** memory allocation failed\n");
         Cleanup();
         return;
    }

   //calculate the length of the hash
    if(!NT_SUCCESS(status = BCryptGetProperty(
                                        hAlg, 
                                        BCRYPT_HASH_LENGTH, 
                                        (PBYTE)&cbHash, 
                                        sizeof(DWORD), 
                                        &cbData, 
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
         Cleanup();
         return;
    }

    //allocate the hash buffer on the heap
    pbHash = (PBYTE)HeapAlloc (GetProcessHeap (), 0, cbHash);
    if(NULL == pbHash)
    {
        wprintf(L"**** memory allocation failed\n");
        err=true;
        Cleanup();
        return;
    }

    //create a hash
    if(!NT_SUCCESS(status = BCryptCreateHash(
                                        hAlg, 
                                        &hHash, 
                                        pbHashObject, 
                                        cbHashObject, 
                                        NULL, 
                                        0, 
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptCreateHash\n", status);
        err=true;
       Cleanup();
       return;
    }

}


WinHash::~WinHash(){
    err = true;
    Cleanup();
}

BOOL WinHash::Update(BYTE* data, size_t dataLen){
        //Update the internal hash state 
    if(!NT_SUCCESS(status = BCryptHashData(
                                        hHash,
                                        data,
                                        dataLen,
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptHashData\n", status);
        err=true;
       Cleanup();
       return FALSE;
    }

    return TRUE;
}

// Compute the digest 
BOOL WinHash::Digest(){
     //close the hash
    if(!NT_SUCCESS(status = BCryptFinishHash(
                                        hHash, 
                                        pbHash, 
                                        cbHash, 
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
        err=true;
       Cleanup();
       return FALSE;
    }

    wprintf(L"Success!\n");
    Cleanup();
     // as per the docmunetation, once the FinalHash is created, the handles to the algorithm cannot be resued!
    return TRUE;
}


// The caller, is responsible for cleaninig up memory.
BYTE* HashData(BYTE* data, size_t dataLen){
     BCRYPT_ALG_HANDLE       hAlg            = NULL;
    BCRYPT_HASH_HANDLE      hHash           = NULL;
    NTSTATUS                status          = STATUS_UNSUCCESSFUL;
    DWORD                   cbData          = 0,
                            cbHash          = 0,
                            cbHashObject    = 0;
    BYTE*                   pbHashObject    = NULL;
    BYTE*                   pbHash          = NULL;
    bool err = false;

    //open an algorithm handle
    if(!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                                                &hAlg,
                                                BCRYPT_SHA256_ALGORITHM,
                                                NULL,
                                                0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
        goto Cleanup;
    }

    //calculate the size of the buffer to hold the hash object
    if(!NT_SUCCESS(status = BCryptGetProperty(
                                        hAlg, 
                                        BCRYPT_OBJECT_LENGTH, 
                                        (PBYTE)&cbHashObject, 
                                        sizeof(DWORD), 
                                        &cbData, 
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        goto Cleanup;
    }

    //allocate the hash object on the heap
    pbHashObject = (PBYTE)HeapAlloc (GetProcessHeap (), 0, cbHashObject);
    if(NULL == pbHashObject)
    {
        wprintf(L"**** memory allocation failed\n");
        goto Cleanup;
    }

   //calculate the length of the hash
    if(!NT_SUCCESS(status = BCryptGetProperty(
                                        hAlg, 
                                        BCRYPT_HASH_LENGTH, 
                                        (PBYTE)&cbHash, 
                                        sizeof(DWORD), 
                                        &cbData, 
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        goto Cleanup;
    }

    //allocate the hash buffer on the heap
    pbHash = (PBYTE)HeapAlloc (GetProcessHeap (), 0, cbHash);
    if(NULL == pbHash)
    {
        wprintf(L"**** memory allocation failed\n");
        err=true;
        goto Cleanup;
    }

    //create a hash
    if(!NT_SUCCESS(status = BCryptCreateHash(
                                        hAlg, 
                                        &hHash, 
                                        pbHashObject, 
                                        cbHashObject, 
                                        NULL, 
                                        0, 
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptCreateHash\n", status);
        err=true;
        goto Cleanup;
    }
    

    //hash some data
    if(!NT_SUCCESS(status = BCryptHashData(
                                        hHash,
                                        (PBYTE)data,
                                        dataLen,
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptHashData\n", status);
        err=true;
        goto Cleanup;
    }
    
    //close the hash
    if(!NT_SUCCESS(status = BCryptFinishHash(
                                        hHash, 
                                        pbHash, 
                                        cbHash, 
                                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
        err=true;
        goto Cleanup;
    }
    for(size_t i = 0; i < cbHash; i++){
        wprintf(L"%02x",pbHash[i] );
    }
    wprintf(L"\n");
    wprintf(L"Success!\n" );

Cleanup:

    if(hAlg)
    {   
        wprintf(L"Cleaned up AlgorithmProvider\n");
        BCryptCloseAlgorithmProvider(hAlg,0);
    }

    if (hHash)    
    {   
        
        BCryptDestroyHash(hHash);
         wprintf(L"Destroyed Hash Object\n");
    }

    if(pbHashObject)
    {
        HeapFree(GetProcessHeap(), 0, pbHashObject);
         wprintf(L"Freed Heap Space used for Hash Object\n");
    }

    if(pbHash && err)
    {
        HeapFree(GetProcessHeap(), 0, pbHash);
         wprintf(L"Cleaned up space used for digest\n");
        pbHash = NULL;
    }
    return pbHash;

}


void wmain(){

     char test[] = "admin";
    auto size = strlen(test);
   
    auto sha256 = new WinHash((LPCWSTR) BCRYPT_SHA256_ALGORITHM);
    sha256->Update( (BYTE*) test, size);
    //sha256->Update( (BYTE*) test, size);
    sha256->Digest();
    wprintf(L"Printing the hash!\n");
    for(DWORD i=0; i < sha256->cbHash; i++){
        wprintf(L"%02x", sha256->pbHash[i]);
    }
    wprintf(L"\n");
    delete  sha256;
    wprintf(L"Goodbye\n");
}