#include <windows.h>
#include <compressapi.h>
#include <stdio.h>

// don't forget to link against Cabinet.lib


int main(){
    char data[]= "hello there!";
    size_t dataLen = strlen(data) + 1;
    BYTE* buffer = NULL;
    size_t bufSize = 0;
    size_t cDataSize = 0;

    COMPRESSOR_HANDLE hCompressor = NULL;
    if(!::CreateCompressor(
        COMPRESS_ALGORITHM_LZMS,
        NULL,
        &hCompressor
    )){
        printf("Could not get Compressor Handle %d\n", ::GetLastError());
    }

/*
BOOL Compress(
  [in]  COMPRESSOR_HANDLE CompressorHandle,
  [in]  LPCVOID           UncompressedData,
  [in]  SIZE_T            UncompressedDataSize,
  [out] PVOID             CompressedBuffer,
  [in]  SIZE_T            CompressedBufferSize,
  [out] PSIZE_T           CompressedDataSize
);
*/
    if(!::Compress(
        hCompressor, 
        (VOID*) data, 
        dataLen,
        (VOID*) buffer,
         bufSize,
         &cDataSize
    )){
        auto err = ::GetLastError();
        if(err != ERROR_INSUFFICIENT_BUFFER){
            printf("Panic %d\n",err );
            return 0; 
        }
    }
   
    buffer = new BYTE[cDataSize];
    bufSize = cDataSize;
     if(!::Compress(
        hCompressor, 
        (VOID*) data, 
        dataLen,
        (VOID*) buffer,
         bufSize,
         &cDataSize
    )){
        printf("Panic %d\n",::GetLastError() );
        return 0;
    }

    for(size_t i =0; i < bufSize; i++){
        printf("%02x", buffer[i]);
    }
    printf("\n");
}