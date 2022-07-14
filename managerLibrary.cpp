#include "managerLibrary.h"

managerLibrary::managerLibrary(){
    wiringPiSetupGpio();
}

string managerLibrary::getGpsPosition(){
    string fullDate = "";
    
    cout << "GPS sync..." << endl;
    int sfd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY);
    if(sfd == -1){
        cout << "Error no is: " << errno << endl;
        cout << "Error descpription is: " << strerror(errno) << endl;
        close(sfd);
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
        
    }
    return "";
}

void managerLibrary::setTime(string currTime){
    string command = "timedatectl set-time \""+currTime+"\""; 
    try{
        system(command.c_str());
    }
    catch(...){
        cout << "Setting time went wrong" << endl; 
    }
    cout << "Time set" << endl;
}

bool managerLibrary::checkDayInCalendar(string currYearString){
    bool freeDay = false;
    getStdout* getOutput = new getStdout();
    //Gauss' Calendar Formula
    int currYear = atoi(currYearString.c_str());
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
    string esterMonth = "";
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
    string esterMondayMonth = esterMonth;
    if(esterMondayDay == 32){
        esterMondayDay = 1;
        esterMondayMonth = "04";
    }
    string esterMondayDayString;
    if(esterMondayDay < 10){
        esterMondayDayString= "0"+to_string(esterMondayDay);
    }
    else{
        esterMondayDayString = to_string(esterMondayDay);
    }
    string esterDayString;
    if(esterDay < 10){
        esterDayString = "0"+to_string(esterDay);
    }
    else{
        esterDayString = to_string(esterDay);
    }

    string command = "date --date \""+currYearString+"-"+esterMonth+"-"+esterDayString+"\" +%s";
    string commandAns;
    try{
        commandAns = getOutput->getStdoutFromSystem(command);
    }
    catch(...){
        cout << "Finding time of Ester in secounds failed" << endl;
    }
    int dateInSec = atoi(commandAns.c_str()) + (60*24*60*60);

    command = "date --date='@"+to_string(dateInSec)+"' +%d%m";
    string dateOfCorpusChrisi;
    try{
        dateOfCorpusChrisi = getOutput->getStdoutFromSystem(command);
    }
    catch(...){
        cout << "Finding date of Corpus Chrisi failed" << endl;
    }
    
    string corpusChrisiDay = dateOfCorpusChrisi.substr(0,2);
    string corpusChrisiMonth = dateOfCorpusChrisi.substr(2,2);

    string esterDate[2] = {esterDayString, esterMonth};
    string esterMonday[2] = {esterMondayDayString, esterMondayMonth};
    string corpusChrisiDate[2] = {corpusChrisiDay, corpusChrisiMonth};

    string freeDays[12][2] = {{"01", "01"},{"06", "01"}, {esterDate[0],esterDate[1]}, {esterMonday[0], esterMonday[1]}, {"01", "05"}, {"03", "05"}, {corpusChrisiDate[0], corpusChrisiDate[1]}, {"15", "08"}, {"01", "11"}, {"11", "11"}, {"25", "12"}, {"26", "12"}};

    command = "date +%a";
    string currPiDayString;
    try{
        currPiDayString = getOutput->getStdoutFromSystem(command);
    }
    catch(...){
        cout << "Finding current day failed" << endl;
    }

    command = "date +%d";
    string currPiDay;
    try{
        currPiDay = getOutput->getStdoutFromSystem(command);
    }
    catch(...){
        cout << "Finding current day number failed" << endl;
    }
    
    if(atoi(currPiDay.c_str())<10){
        currPiDay = "0"+currPiDay;
    }

    command = "date +%m";
    string currPiMonth;
    try{
        currPiMonth = getOutput->getStdoutFromSystem(command);
    }
    catch(...){
        cout << "Finding current month failed" << endl;
    }
    
    if(atoi(currPiMonth.c_str())<10){
        currPiMonth = "0"+currPiMonth;
    }


    for(int i=0; i<12; i++){
        if(currPiDay == freeDays[i][0] && currPiMonth == freeDays[i][1]){
            freeDay = true;
        }
        else if(currPiDayString == "Sun"){
            freeDay = true;
        }
        else{
            freeDay = false;
        }
    }

    return freeDay;

}
void managerLibrary::turnGPIOs(int firstGpio, int secoundGpio, bool mode){

    try{
        pinMode(firstGpio, OUTPUT);
        if(mode){
            digitalWrite(firstGpio, HIGH);
        }
        else{
            digitalWrite(firstGpio, LOW);
        }
        
    }
    catch(...){
        cout << "Turning first gpio went wrong." << endl;
    }

    try{
        pinMode(secoundGpio, OUTPUT);
        if(mode){
            digitalWrite(secoundGpio, HIGH);
        }
        else{
            digitalWrite(secoundGpio, LOW);
        }
    }
    catch(...){
        cout << "Turning secound gpio went wrong." << endl;
    }
    
    

}
int** managerLibrary::testGPIOs(){

    int** chanList;
    chanList = new int * [7];

    chanList[0] = new int [2];
    chanList[1] = new int [2];
    chanList[2] = new int [2];
    chanList[3] = new int [2];
    chanList[4] = new int [2];
    chanList[5] = new int [2];
    chanList[6] = new int [2];

    chanList[0][0] = 17;
    chanList[0][1] = 1;
    chanList[1][0] = 22;
    chanList[1][1] = 1;
    chanList[2][0] = 23;
    chanList[2][1] = 1;
    chanList[3][0] = 24;
    chanList[3][1] = 1;
    chanList[4][0] = 25;
    chanList[4][1] = 1;
    chanList[5][0] = 27;
    chanList[5][1] = 1;
    chanList[6][0] = 5;
    chanList[6][1] = 1;

    for( int i=0; i<7; i++){
        try{
            pinMode(chanList[i][0], OUTPUT);
            digitalWrite(chanList[i][0], HIGH);
            digitalWrite(chanList[i][0], LOW);
        }
        catch(...){
            cout << "Configuring pin " << chanList[i] << " failed" << endl;
            chanList[i][1] = 0;
        }
    }
    return chanList;

}
bool managerLibrary::checkTimeToSync(){
    string command = "date +%H";
    getStdout* getOutput = new getStdout();
    string currPiHour;
    try{
        currPiHour = getOutput->getStdoutFromSystem(command);
    }
    catch(...){
        cout << "Finding current time failed" << endl;
    }

    if(atoi(currPiHour.c_str())<10){
        currPiHour = "0" + currPiHour;
    }

    if(currPiHour == "04"){
        return true;
    }
    else{
        return false;
    }
}
gpiosType managerLibrary::config(){
    bool configuration = false;

    int ** chanList;

    chanList = testGPIOs();
    
    int firstGpio, secoundGpio;
    cout << endl;
    cout << "Welcome in configuration." << endl;
    cout << "Avaliable GPIOs: ";
    for(int i=0; i<7; i++){
        if(chanList[i][1] == 1){
            cout << "\t" << chanList[i][0];
        }
    }
    
    while(configuration==false){
        cout << endl;
        cout << "Choose first GPIO:";
        cin >> firstGpio;
        cin.ignore();
        for(int i=0; i<7; i++){
            if(chanList[i][0] == firstGpio && chanList[i][1] == 1){
                chanList[i][1] = 0;
                configuration = true;
                choosedGpios.firstGpio = chanList[i][0];
                break;
            }
        }
        cout << endl;
        cout << "Avaliable GPIOs: ";
        for(int i=0; i<7; i++){
            if(chanList[i][1] == 1){
                cout << "\t" << chanList[i][0];
            }
        }
        cout << endl;
        cout << "Choose secound GPIO: ";
        cin >> secoundGpio;
        cin.ignore();
        for(int i=0; i<7; i++){
            if(chanList[i][0] == secoundGpio && chanList[i][1] == 1){
                chanList[i][1] = 0;
                if(configuration == true){
                    choosedGpios.secoundGpio = chanList[i][0];
                    break;
                }
                else{
                    cout << "One of choosed GPIOs is incorrect. Try again." << endl;
                    chanList = testGPIOs();
                }
            }
        }
    }
    cout << endl;
    cout << "Configuration done.";
    for(int i=0; i<7; i++){
        delete [] chanList[i];
    }
    delete [] chanList;

    return choosedGpios;
}