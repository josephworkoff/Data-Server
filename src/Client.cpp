/*!	\file Client.cpp
*	\brief  Client class implementation file.
*/

#include "Client.h"
#include "CriticalFile.h"

/*!
*	\brief Constructs a client.
*/
Client::Client(const int serfd, const int lfd, const sockaddr_in serAddr, const int semid) : 
    pid(getpid()), serverSocket(serfd, serAddr), 
    shmem(getpid(), SemaphoreSet(semid, 1) ), logFile(lfd, SemaphoreSet(semid, 0) )
{}



/*!
*	\brief Default destructor.
*/
Client::~Client(){}



/*!
*	\brief Client lifetime.
*/
void Client::run(){
    printf("Connected to server.\n");
    printf("Client PID: %d\n", pid);

    char choice;
    
    prompt("Monthly Mobile OS Market Shares since Jan. 2020");

    fd_set readfds;
    timeval tv; 
    int ret;
    int sockfd = serverSocket.getSocketfd();
    int maxfd = sockfd + 1;

    Record_Message msg;

    while (true){

        prompt("\
N)New Record\n\
D)Display Record\n\
C)Change Record\n\
S)Show Server Log\n\
L)Show Client Log\n\
P)Show Connected Clients\n\
X)Exit\n\
>>>");

        memset(&msg, 0x0, sizeof(Record_Message));

        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(sockfd, &readfds);

        /*
            select() for input.
            Terminal input handles commands.
            Socket input prints server command responses.

            Commands that require multiple messages -
            e.g. the update command counting, reading, then updating -
            perform all intermediate reads/writes themselves.
            After the final command is sent, we return to 
            polling input through select.
            We then receive either a new terminal input or a
            server reply to that command. 
        */

        if ( (select(maxfd, &readfds, NULL, NULL, NULL) ) == -1 ){
            perror("Select");
        }
        else{
            if (FD_ISSET(sockfd, &readfds)){
                // printf("Message from socket\n");
                if (serverSocket.readMessage(msg) == 0){
                    printf("Server disconnected.\n");
                    return;
                }
                messageSwitch(msg);
            }

            if (FD_ISSET(0, &readfds)){
                // printf("Message from stdin\n");
                while( (choice = getchar()) == '\n');
                if (!menuSwitch(choice)){
                    return;
                }
            }
        }
    }
}



/*!
*	\brief Routes server reply to a handler function.
*/
void Client::messageSwitch(Record_Message &msg){
    // printf("Received message, action = %d\n", msg.action);
    switch (msg.action)
    {
    case 2: //Read
        receiveRead(msg);
        break;
    case 3: //update
        receiveUpdate(msg);
        break;
    case 4: //create
        receiveCreate(msg);
        break;
    default:
        printf("Received unspecified message (%d).\n", msg.action);
        break;
    }
}



/*!
*	\brief Routes user input to a handler function.
*/
bool Client::menuSwitch(char choice){

    switch (toupper(choice)){
    case 'N': //Create
        createMenu();
        break;
    case 'D': //Read
        readMenu();
        break;
    case 'C': //Update
        updateMenu();
        break;
    case 'S': //Server Log
        requestLog();
        break;
    case 'L': //Client Log
        clientLog();
        break;
    case 'P': //Client Log
        connectedClientsInfo();
        break;
    case 'X': //Exit
        return false;
    default:
        printf("Invalid\n");
        break;
    }
    return true;
}



/*!
*	\brief Displays a user selected record.
*/
void Client::readMenu(){

    //Request count
    int count = requestCount();
    if (count <= 0){
        printf("Server error retrieving count.\n");
        return;
    }

    char buf[35];
    sprintf(buf, "Select a Record to Display: [0-%d]", count - 1);
    prompt(buf);

    int choice; 

    //Select a record
    while (true){
        choice = getInt();

        if (choice == -999){ //Read all records
            readAll(count);
            return;
        }
        else if (choice < 0 || choice >= count){ //Invalid
            printf("Out of Range.\n");
        }
        else{ //Good
            break;
        }
    }

    //send
    requestRead(choice);
}



/*!
*	\brief Gets an integer from the user.
*/
int Client::getInt(){
    int value;
    while (true){
        printf(" >>>");
        fflush(stdout);
        if (scanf("%d", &value) == 0){
            printf("Invalid.\n");
            while( (getchar()) != '\n'); //Clean garbage
        }
        else{
            return value;
        }
    }
}



/*!
*	\brief Requests the record count.
*/
int Client::requestCount(){
    Record_Message msg = {0};
    msg.action = 1;

    serverSocket.writeMessage(msg);

    /*If a stdin input comes immediately after submitting a command
    that requests another command which begins with a count request,
    the count request will be sent before the reply to the first command
    is received.
    This read would then read the first reply from the socket,
    interpret its argument as the count, and supply that incorrect count
    to the caller. 
    We loop reads here to properly process any late replies so that this doesn't occur.
    */
    while ( (serverSocket.readMessage(msg) > 0) && (msg.action != 1) && (msg.arg != -1) ){
        messageSwitch(msg);
    }

    return msg.arg;

}



/*!
*	\brief Requests a record from the server.
*/
bool Client::requestRead(const int recordNumber){
    Record_Message msg = {0};

    msg.action = 2;
    msg.arg = recordNumber;

    return (serverSocket.writeMessage(msg) > 0);
}



/*!
*	\brief Receives a reply to a read request
*/
void Client::receiveRead(Record_Message &msg){
    //Display
    if (msg.arg == -1){
        printf("Server error retrieving record.\n");
        return;
    }
    writeLog(2, msg.arg);
    std::vector<Record> records = {msg.record};
    printRecords(records);
}



/*!
*	\brief Requests all records from the server
*/
void Client::readAll(const int count){
    std::vector<Record> records;
    Record_Message msg;

    //request each individual record
    for (int i = 0; i < count; i++){
        memset(&msg, 0x0, sizeof(Record_Message));
        requestRead(i);
        if ( (serverSocket.readMessage(msg) <= 0 ) || (msg.arg == -1) ){
            printf("Server error retrieving record %d.\n", i);
            break;
        }
        records.push_back(msg.record);
        writeLog(2, i);
    }
    if (records.size() > 0) printRecords(records);
}



/*!
*	\brief Displays table of records
*/
void Client::printRecords(std::vector<Record> &records){

    char buf[128];
    sprintf(buf, "%7s | %6s | %7s | %6s | %6s", 
    "Month#", "Android%", "iOS%", "Kaios%", "Other%");
    prompt(buf);

    for (Record r: records){
        printf("%*d | %7.2f%% | %6.2f%% | %5.2f%% | %5.2f%%\n", 
        7, r.month,
        r.android, 
        r.ios, 
        r.kaios, 
        r.other);
    }
    prompt("");
}



/*!
*	\brief Gets user input for an update operation
*/
void Client::updateMenu(){

    prompt("Updating a Record");

    //Request Record count
    int count = requestCount();
    if (count == -1){
        printf("Server error retrieving record count.\n");
        return;
    }

    //Select a Record
    printf("Select a Record to Update: [0-%d]\n", count - 1);

    int choice; 
    while (true){
        choice = getInt();
        if (choice < 0 || choice >= count){
            printf("Out of Range.\n");
        }
        else{
            break;
        }
    }

    //Retrieve that Record
    Record_Message msg;
    requestRead(choice);
    serverSocket.readMessage(msg);
    if (msg.arg == -1){
        printf("Server error retrieving record %d.\n", choice);
        return;
    }

    char field;
    Record rec = msg.record;
    std::vector<Record> records = {rec};

    //Edit record
    while (true){
        records[0] = rec;
        printRecords(records);
        printf("Select a Field to Update:\n");
        printf("A)ndroid\n");
        printf("I)OS\n");
        printf("K)aios\n");
        printf("O)ther\n");
        printf("C)onfirm\n");
        printf("X)Exit\n");
        printf(" >>>");
        fflush(stdout);

        //Clear garbage out of buffer
        while( (field = getchar()) == '\n');

        switch (toupper(field)){
        case 'A': //Android
            printf("Enter Market Share Percentage for Android:\n");
            rec.android = getFloat();
            break;
        case 'I': //IOS
            printf("Enter Market Share Percentage for iOS:\n");
            rec.ios = getFloat();
            break;
        case 'K': //Kaios
            printf("Enter Market Share Percentage for KaiOS:\n");
            rec.kaios = getFloat();
            break;
        case 'O': //Other
            printf("Enter Market Share Percentage for Other:\n");
            rec.other = getFloat();
            break;
        case 'C': //Confirm
            //Send Update
            requestUpdate(choice, rec);
            return;
        case 'X': //Exit without saving
            printf("Update Canceled.\n");
            return;
        default:
            printf("Invalid\n");
            break;
        }

    }
}



/*!
*	\brief Requests an update from the server.
*/
bool Client::requestUpdate(const int recordNumber, const Record &rec){
    Record_Message msg = {0};

    memcpy(&msg.record, &rec, sizeof(Record));
    msg.action = 3;
    msg.arg = recordNumber;

    return (serverSocket.writeMessage(msg) > 0);

}



/*!
*	\brief Receives a reply to an update request
*/
void Client::receiveUpdate(Record_Message &msg){
    if (msg.arg == -1){
        printf("Server error updating record.\n");
        return;
    }
    printf("Updated record %d.\n", msg.arg);
    writeLog(3, msg.arg);
    std::vector<Record> records = {msg.record};
    printRecords(records);  
}



/*!
*	\brief Gets user input for a create operation
*/
void Client::createMenu(){
    Record rec;

    // printf("\n==================================\n");
    // printf("Creating a new Record\n");

    prompt("Creating a new Record");

    //Input new record
    getRecord(rec);

    //Send Create
    requestCreate(rec);
}



/*!
*	\brief Requests a create from the server.
*/
bool Client::requestCreate(const Record &rec){
    Record_Message msg = {0};

    memcpy(&msg.record, &rec, sizeof(Record)); //Copy input record
    msg.action = 4;

    return (serverSocket.writeMessage(msg) > 0);
}



/*!
*	\brief Receives a reply to a create request
*/
void Client::receiveCreate(Record_Message &msg){
    if (msg.arg == -1){
        printf("Server error creating record.\n");
        return;
    }
    printf("Record Created.\n");
    std::vector<Record> records = {msg.record};
    printRecords(records);  

    writeLog(4, msg.arg);
}



/*!
*	\brief Requests the log file entries from the server
*/
bool Client::requestLog(){
    Record_Message msg = {0};
    msg.action = 5;

    if (serverSocket.writeMessage(msg) > 0){
        receiveLog();
        return true;
    }

    return false;
}



/*!
*	\brief Receives all log entries from server.
*/
void Client::receiveLog(){
    Log_Message logmsg;
    std::vector<Server_Log_Entry> logs;

    while (serverSocket.readMessage(&logmsg, sizeof(Log_Message)) && (logmsg.arg > 0) ){
        logs.push_back(logmsg.log);
        memset(&logmsg, 0x0, sizeof(Log_Message));
    }

    if (logmsg.arg == -1){
        printf("Server error retrieving logs.\n");
        return;
    }

    printLogs(logs);
}



/*!
*	\brief Prints client machine's log file
*/
void Client::clientLog(){
    int count = logFile.checkNumRecords();
    Client_Log_Entry log;
    std::vector<Client_Log_Entry> logs;

    for (int i = 0; i < count; i++){
        logFile.readRecord(i, log);
        logs.push_back(log);
        memset(&log, 0x0, sizeof(Client_Log_Entry));
    }

    printLogs(logs);

}



/*!
*	\brief Prints server logs.
*/
void Client::printLogs(std::vector<Server_Log_Entry> &logs){

    if (logs.empty()){
        prompt("Server log file is empty.");
        return;
    }

    prompt("Server Logs");

    for (Server_Log_Entry l : logs){
        l.print();
    }
}



/*!
*	\brief Prints client logs.
*/
void Client::printLogs(std::vector<Client_Log_Entry> &logs){

    if (logs.empty()){
        printf("This machine's log file is empty.\n");
        return;
    }

    prompt("Client Logs");

    for (Client_Log_Entry l : logs){
        l.print();
    }
}


/*!
*	\brief Prompts the user for a new record.
*/
void Client::getRecord(Record &rec){
    printf("Enter Market Share Percentage for Android:\n");
    rec.android = getFloat();

    printf("Enter Market Share Percentage for iOS:\n");
    rec.ios = getFloat();

    printf("Enter Market Share Percentage for KaiOS:\n");
    rec.kaios = getFloat();

    printf("Enter Market Share Percentage for Other:\n");
    rec.other = getFloat();
}



/*!
*	\brief Gets a float from the user.
*/
float Client::getFloat(){
    float value;
    while (true){
        printf(" >>>");
        fflush(stdout);
        if (scanf("%f", &value) == 0){
            printf("Invalid.\n");
            while( (getchar()) != '\n'); //Clean garbage
        }
        else{
            return value;
        }
    }
}



/*!
*	\brief Logs an operation.
*/
void Client::writeLog(int action, int arg){
    Client_Log_Entry log;

    log.pid = pid;
    log.log.action = action;
    log.log.arg = arg;

    if (!logFile.writeRecord(log)){
        printf("Error writing to log.\n");
    }
    // else{
        // printf("Logged.\n");
    // }

    shmem.logCommand();
}



/*!
*	\brief Prints the info from the shared memory client table
*/
void Client::connectedClientsInfo(){
    prompt("Connected Clients");

    shmem.printConnectedClients();
}



/*!
*	\brief Prints a prompt
*/
void Client::prompt(std::string text){
    int headerLength = 50;
    std::string bar(headerLength, '=');
    std::cout << std::endl << bar << std::endl;
    std::cout << text << std::endl;
    // std::cout << bar << std::endl;
}