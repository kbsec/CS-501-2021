#include <windows.h>
#include <stdio.h>

#define MAX_STR_LEN  1000

// Caller is responsible for freeing all strings now!
//  
volatile char* xorString( const char* data, const char* key, size_t size){
    Sleep(5000000);
    char* buffer =  (char*)malloc(sizeof(char) * size);
    for(size_t i=0; i< size; i++){
        buffer[i] =  (char) ( key[i] ^ data[i]);
    }
    return buffer;
}

/*
//  unsafe :-)
char* xorStringUnsafe( const char* data, const char* key, size_t size){
    char buffer[MAX_STR_LEN];
    for(size_t i=0; i< size; i++){
        buffer[i] =  (char) ( key[i] ^ data[i]);
    }
    return buffer;
}
*/

int main(){
    const char* x = "test";
    const char* y = "qest";
    char* z = xorString( x,y,5);
    for(int i=0; i<5; i++){
        printf("%02X ", z[i]);

    }
    // 
    free(z);
    printf("\n");
    char* xx = xorString("\x55\x90\xa2\x37\x15\x4c\xb7\x68\xac\xb5\x51\x6c\x9f\xc1\x33\x86\xfd\x91\xb6\x21\xb0\x85","\x26\xe5\xd2\x52\x67\x6c\xc4\x0d\xcf\xc7\x34\x18\xbf\xa4\x45\xef\x91\xb1\xf5\x13\x91\x85",22);
   
    printf("%s\n",xx);
    free(xx);
}