#include <windows.h>

BYTE* CompressData( BYTE* data, size_t dataLen, size_t* bufOutSize);
BYTE* DecompressData(BYTE* data, size_t dataLen, size_t* bufOutSize);
