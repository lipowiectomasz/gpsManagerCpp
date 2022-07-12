#include "managerLibrary.h"

managerLibrary::managerLibrary(){}

string managerLibrary::getGpsPosition(){
    string fullDate = "";

    cout << "GPS sync..." << endl;
    int sfd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY);
    if(sfd == -1){
        cout << "Error no is: " << errno << endl;
        cout << "Error descpription is: " << strerror(errno) << endl;
    }
    else{
        char response[80];
        int count = 0;
        string responseAsString = "";
        string wantedNav = "RMC";
        size_t wantedNavPos = 0;
        string hour = "";
        string date = "";
        
        string msg = "";
        string firstState = ",,V";
        string secoundState = ",V,,,,,,,,,,";
        string thirdState = ",V,,,,,,,";
        string fourthState = ",,,";
        int datePos = 0;

        do{
            count = read(sfd, response, 80);
            responseAsString = string(response);
            wantedNavPos = responseAsString.find(wantedNav);
            if(wantedNavPos != string::npos){
                
                if(responseAsString.find(firstState) != string::npos ||responseAsString.find(secoundState) != string::npos){
                    cout << "." << flush;
                    delay(1000);
                    close(sfd);
                    sfd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY);
                    continue;
                }
                if(responseAsString.find(thirdState) != string::npos){
                    datePos = 22 + int(wantedNavPos);
                }
                else if(responseAsString.find(fourthState) != string::npos){
                    datePos = int(responseAsString.find(fourthState))-6;
                }
                else{
                    datePos = int(wantedNavPos) + 50;
                }

                hour = responseAsString.substr(int(wantedNavPos)+4,6);
                date = responseAsString.substr(datePos,6);
                int correctedHour = atoi(hour.substr(0,2).c_str())+2;
                string correctedHourString = "";
                if(correctedHour<10){
                    correctedHourString = "0"+to_string(correctedHour);
                }
                else{
                    correctedHourString = to_string(correctedHour);
                }
                hour.replace(0,2,correctedHourString);
                fullDate = date+hour;
                fullDate.insert(0,"20");
                fullDate.insert(4,"-");
                fullDate.insert(7,"-");
                fullDate.insert(10," ");
                fullDate.insert(13,":");
                fullDate.insert(16,":");
                string dateYear = date.substr(4,2);
                string dateDay = date.substr(0,2);
                fullDate.replace(2,2,dateYear);
                fullDate.replace(8,2,dateDay);
                cout << endl;
                cout << "Found date: " << fullDate << endl;
                close(sfd);
                return fullDate;
                break;
                
        
            }
            else{
                cout << "." << flush;
                delay(2000);
            }
        }while(1);
        close(sfd);
    }
    return fullDate;
}

void managerLibrary::setTime(string currTime){
    string command = "timedatectl set-time "+currTime; 
    try{
        system(command.c_str());
    }
    catch(...){
        cout << "Setting time went wrong" << endl; 
    }
    cout << "Time set" << endl;
}

bool managerLibrary::checkDayInCalendar(){

    //Gauss' Calendar Formula
    int currYear;
    int k = 24;
    int l = 5;
    int a = currYear % 19;
    int b = currYear % 4;
    int c = currYear % 7;
    int d = (19*a+k) % 30;
    int e = (2*b+4*c+6*d+l) % 7;

    int esterInApril = 0;
    int esterInMarch = 0;
    int esterDay = 0;
    if(d == 29 && e == 6){
        esterInApril = 19;
    }
    else if(d == 28 && e == 6){
        esterInApril = 18;
    }
    else{
        esterInMarch = 22+d+e;
        esterInApril = d+e-9;
        if(esterInMarch > 31){
            esterDay = esterInApril;
            esterMonth = "04";
        }
        else{
            esterDay = esterInMarch;
            esterMonth = "03";
        }
    }
    int esterMondayDay = esterDay + 1;
    int esterMondayMonth = esterMonth;
    
    if(esterMondayDay == 32){
        esterMondayDay = 1;
        esterMondayMonth = "04";
    }

    if(esterMondayDay < 10){
        esterMondayDay = "0"+to_string(esterMondayDay)
    }
    else{
        esterMondayDay = to_string(esterMondayDay);
    }

    if(esterDay < 10){
        esterDay = "0"+to_string(esterDay);
    }
    else{
        esterDay = to_string(esterDay);
    }
    currYear = currYear.substr(0,4);

    string command = "date --date \""+currYear+"-"+esterMonth+"-"+esterDay+"\" +%s";
    try{
        int commandAns = system(command.c_str()); 
    }
    catch(...){
        cout << "Finding time of Ester in secounds failed" << endl;
    }
    int dateInSec = commandAns + (60*24*60*60);

    command = "date --date='@"+dateInSec+"' +%d%m";
    try{
        int dateOfCorpusChrisi = system(command.c_str());
    }
    catch(...){
        cout << "Finding date of Corpus Chrisi failed" << endl;
    }

    string corpusChrisiDay = dateOfCorpusChrisi.substr(0,2);
    string corpusChrisiMonth = dateOfCorpusChrisi.substr(2,2);

    string esterDate[2] = {esterDay, esterMonth};
    string esterMonday[2] = {esterMondayDay, esterMondayMonth};
    string corpusChrisiDate[2] = {corpusChrisiDay, corpusChrisiMonth};

    string freeDays[12][2] = {{"01", "01"},{"06", "01"}, esterDate, esterMonday, {"01", "05"}, {"03", "05"}, corpusChrisiDate, {"15", "08"}, {"01", "11"}, {"11", "11"}, {"25", "12"}, {"26", "12"}};

    command = "date +%a";
    try{
        int currPiDay = system(command.c_str());
    }
    catch(...){
        cout << "Finding current day failed" << endl;
    }

    for(int i=0; i<freeDays.length(); i++){
        if(currDay == freeDays[i][0] && currMonth == freeDays[i][1]){
            return true;
        }
        else if(to_string(currPiDay) == "Sun"){
            return true;
        }
        else{
            return false;
        }
    }

    return true;

}
void managerLibrary::turnOnGPIOs(int firstGpio, int secoundGpio){
    try{
        pinMode(firstGpio, OUTPUT);
        digitalWrite(firstGpio, HIGH);
    }
    catch(...){
        cout << "Turning first gpio went wwrong." << endl;
    }

    try{
        pinMode(firstGpio, OUTPUT);
        digitalWrite(firstGpio, HIGH);
    }
    catch(...){
        cout << "Turning secound gpio went wwrong." << endl;
    }
    
    cout << "Gpio's on" << endl;

}
int managerLibrary::testGPIOs(){

    int chanList[7][2] = {{11,1}, {13,1}, {15,1}, {16,1}, {18,1}, {22,1}, {29,1}};
    for( int i=0; i<chanList.length(); i++){
        try{
            pinMode(chanList[i][0], OUTPUT);
            digitalWrite(chanList[i][0], HIGH);
        }
        catch(...){
            cout << "Configuring pin " << chanList[i] << " failed" << endl;
            chanList[i][1] = 0;
        }
    }
    return &chanList;

}
bool managerLibrary::checkTimeToSync(){
    string command = "date +%H";
    try{
        int currPiHour = system(command.c_str());
        cout << "CurrPiHour in int: " << currPiHour << endl;
        cout << "CurrPiHour in string: " << to_string(currPiHour) << endl;
    }
    catch(...){
        cout << "Finding current time failed" << endl;
    }

    if(to_string(currPiHour) == "00"){
        return true
    }
    else{
        return false;
    }
}

void managerLibrary::config(int *chanList){
    bool configuration = false;

    int firstGpio, secoundGpio;

    cout << "Avaliable GPIOs: ";
    for(int i=0; i<chanList.length(); i++){
        if(chanList[i][1] == 1){
            cout << "\t" << chanList[i][0];
        }
    }
    cout << endl;
    while(configuration==false){
        cout << "Choose first GPIO: ";
        cin >> firstGpio;
        for(int i=0; i<chanList.length(); i++){
            if(chanList[i][0] == firstGpio && chanList[i][1] == 1){
                chanList[i][1] = 0;
                configuration = true;
                break;
            }
        }

        cout << "Avaliable GPIOs: ";
        for(int i=0; i<chanList.length(); i++){
            if(chanList[i][1] == 1){
                cout << "\t" << chanList[i][0];
            }
        }
        cout << "Choose secound GPIO: ";
        cin >> secoundGpio;
        for(int i=0; i<chanList.length(); i++){
            if(chanList[i][0] == firstGpio && chanList[i][1] == 1){
                chanList[i][1] = 0;
                if(configuration == true){
                    turnOnGPIOs(firstGpio, secoundGpio);
                    break;
                }
                else{
                    cout << "One of choosed GPIOs is incorrect" << endl;
                }
            }
        }
    }
    

}