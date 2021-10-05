#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include "check.h"
void Shell(std::string cmd){
    ChonkyCheck();    
    char buffer[1024];
    char container[1024];
    char total_response[18384];
    ZeroMemory(buffer, sizeof(buffer));
    ZeroMemory(container, sizeof(container));
    ZeroMemory(total_response, sizeof(total_response));

    FILE *fp;
    fp = _popen(cmd.c_str(), "r");
     while (fgets(container, 1024, fp) != NULL)
            {
                strcat(total_response, container);
            }
    printf("Result:\n%s", total_response);
    fclose(fp);
}



int _tmain(){
    Shell("whoami");
    Shell("arp -a");

    return 0;
}