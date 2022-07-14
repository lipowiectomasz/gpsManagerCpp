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
#include <unistd.h>
#include "getStdout.h"

struct gpiosStruct{ int firstGpio, secoundGpio; };
typedef struct gpiosStruct gpiosType;

using namespace std;

class managerLibrary{
    public:
        managerLibrary();
        char sendAT();
        string getGpsPosition();
        void setTime(string currTime);
        bool checkDayInCalendar(string currTime);
        void turnGPIOs(int firstGpio, int secoundGpio, bool mode);
        int** testGPIOs();
        bool checkTimeToSync();
        gpiosType config();
        gpiosType choosedGpios;
};

#endif