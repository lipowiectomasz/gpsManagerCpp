#include "managerLibrary.h"

int main(){
    
    managerLibrary* managerEngine = new managerLibrary();

    string fullDate = managerEngine->getGpsPosition();

    managerEngine->setTime(fullDate);

    bool goToConfig = false;
    int x = 0;

    while(x > 0){
        cout << "Press any key to turn on config " << endl; 
    }


    return 0;
}