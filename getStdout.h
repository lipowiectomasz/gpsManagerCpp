#ifndef GETSTDOUT_H
#define GETSTDOUT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

class getStdout{
    public:
        string getStdoutFromSystem(string command);
};

#endif