#ifndef MANAGERLIBRARY_H
#define MANAGERLIBRARY_H

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string>
#include <cstring>
#include <iostream>
#include <wiringPi.h>
#include <typeinfo>
#include <cstdlib>

using namespace std;

class managerLibrary{
    public:
        managerLibrary();
        char sendAT();
        string getGpsPosition();
        void setTime(string currTime);
        bool checkDayInCalendar();
        void turnOnGPIOs();
        int testGPIOs();
        bool checkTimeToSync();
};

#endif