#include "managerLibrary.h"
#include "kbhit.h"


int main(int argc, char *argv[]){
    
    managerLibrary* managerEngine = new managerLibrary();

    string fullDate = managerEngine->getGpsPosition();

    managerEngine->setTime(fullDate);
    managerEngine->choosedGpios.firstGpio = 17;
    managerEngine->choosedGpios.secoundGpio = 27;

    keyboard keyb;
    int x = 10;


    cout << "Press any key to turn on config "; 
    while(x > 0){
        if(keyb.kbhit())
        {
            managerEngine->choosedGpios = managerEngine->config();
            break;
        }
        sleep(1);
        x--;
        cout << "\rPress any key to turn on config ( " << x << "s )"<< flush;
    }
    cout << endl;
    if(managerEngine->checkDayInCalendar(fullDate.substr(0,4))){
        cout << "Today is free day, gpio's are disable" << endl; 
        managerEngine->turnGPIOs(managerEngine->choosedGpios.firstGpio, managerEngine->choosedGpios.secoundGpio, false);
    }
    else{
        cout << "Today is work day, gpio,s are enable" << endl;
        managerEngine->turnGPIOs(managerEngine->choosedGpios.firstGpio, managerEngine->choosedGpios.secoundGpio, true);
    }
    unsigned char exitMark;
    int time;
    cout << "Press Ctrl+C to stop program" << endl;
    while(true){
        if(managerEngine->checkTimeToSync()){
            fullDate = managerEngine->getGpsPosition();
            managerEngine->setTime(fullDate);

            if(managerEngine->checkDayInCalendar(fullDate.substr(0,4))){
                cout << "Today is free day, gpio's are disable" << endl; 
                managerEngine->turnGPIOs(managerEngine->choosedGpios.firstGpio, managerEngine->choosedGpios.secoundGpio, false);
                cout << "Gpio's off" << endl;
            }
            else{
                cout << "Today is work day, gpio's are enable" << endl;
                managerEngine->turnGPIOs(managerEngine->choosedGpios.firstGpio, managerEngine->choosedGpios.secoundGpio, true);
                cout << "Gpio's on" << endl;
            }

        }
        time=0;
        while(time<3600){
            if(keyb.kbhit()){
                exitMark = keyb.getch();
                if(static_cast <int>(exitMark) == 3){//CTRL+C
                    managerEngine->turnGPIOs(managerEngine->choosedGpios.firstGpio, managerEngine->choosedGpios.secoundGpio, false);
                    cout << "Ending" << endl;
                    return 0;
                } 
            }
            sleep(1);
            time++;
        }
    }

    return 0;
}