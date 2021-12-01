#include <windows.h>
#include <compressapi.h>
#include <stdio.h>


//  Caller is responsible for freeing memory
// Size of the buffer is written to bufOutSize.
BYTE* CompressDataWithHandle(COMPRESSOR_HANDLE hCompressor, BYTE* data, size_t dataLen, size_t* bufOutSize){
    size_t bufSize = 0;
    BYTE* buff = NULL;
    ::Compress(
        hCompressor, 
        (LPVOID) data,
        dataLen, 
        (PVOID) buff, 
        bufSize,
        bufOutSize
    ); // expecte to fail
    buff = (BYTE*) new BYTE[*bufOutSize];
    bufSize = *bufOutSize;
    if(!  ::Compress(
        hCompressor, 
        (LPVOID) data,
        dataLen, 
        (PVOID) buff, 
        bufSize,
        bufOutSize
    )){
        printf("Failed to Compress data: %d\n", ::GetLastError());
        delete [] buff;
        return NULL;
    }
    return buff;
}

BYTE* CompressData( BYTE* data, size_t dataLen, size_t* bufOutSize){
    COMPRESSOR_HANDLE  hCompressor;
    if (!::CreateCompressor(
        COMPRESS_ALGORITHM_LZMS,
        NULL,
        &hCompressor
    )
    ){
        return NULL;
    }
    BYTE* result =  CompressDataWithHandle( hCompressor, data,  dataLen,  bufOutSize);
     ::CloseCompressor (hCompressor);
     return result;
}


BYTE* DecompressDataWithHandle(DECOMPRESSOR_HANDLE  hDeCompressor, BYTE* data, size_t dataLen, size_t* bufOutSize){
    BYTE* buff = NULL;
    size_t buffSize = 0;

    ::Decompress(
        hDeCompressor,
        (LPVOID) data,
        dataLen, 
        (PVOID)buff,
        buffSize, 
        bufOutSize
    );  
    buff = (BYTE*) new BYTE[*bufOutSize];
    buffSize = *bufOutSize;
    if (!::Decompress(
        hDeCompressor,
        (LPVOID) data,
        dataLen, 
        (PVOID)buff,
        buffSize, 
        bufOutSize
    )){
        printf("Failed to Decompress %d\n", ::GetLastError());
        delete [] buff;
        return NULL;
    }
    return buff;
}

BYTE* DecompressData(BYTE* data, size_t dataLen, size_t* bufOutSize){
    DECOMPRESSOR_HANDLE  hDeCompressor;
    if(!::CreateDecompressor( 
        COMPRESS_ALGORITHM_LZMS,
        NULL,
        &hDeCompressor)
        ){
            printf("Failed to create Decompressor %d\n", ::GetLastError());
            return NULL;
        }
    BYTE* result = DecompressDataWithHandle(hDeCompressor, data, dataLen, bufOutSize);
    CloseDecompressor(hDeCompressor);
    return result;
}
