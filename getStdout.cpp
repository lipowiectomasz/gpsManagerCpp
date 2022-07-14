#include "getStdout.h"

string getStdout::getStdoutFromSystem(string command){
    
    string output;
    FILE * shell;
    const int maxBuff = 256;
    char buff[maxBuff];
    command.append(" 2>&1");

    shell = popen(command.c_str(), "r");
    if(shell){
        while(!feof(shell)){
            if(fgets(buff, maxBuff, shell) != NULL){
                output.append(buff);
            }
        }
        pclose(shell);
    }
    
    return output;
}