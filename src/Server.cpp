/*!	\file Server.cpp
*	\brief  Server class implementation file.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: Server.cpp\n
*   \b Purpose: Implement the Server class.\n
*   \n
*
*/

#include "Server.h"
#include "SemaphoreSet.h"



/*!
*	\brief Constructs a data server
*/
Server::Server(int bfd, int clifd, int lfd, sockaddr_in cliAddr, int semid) : 
    /*binfd(bfd), logfd(lfd), */clientSocket(clifd, cliAddr), binFile(bfd, SemaphoreSet(semid, 0) ),
    logFile(lfd, SemaphoreSet(semid, 1) ){}



/*!
*	\brief Default destructor
*/
Server::~Server() {}



/*!
*	\brief Child data server lifetime.
*/
void Server::run(){
    printf("%d: Child connected to: %s : %d\n", getpid(), clientSocket.getipaddr(), clientSocket.getPort());
    writeLog(5, 0);

    Record_Message msg;
    int r;

    //block all signals
    sigset_t sigset, oldset;
    sigfillset(&sigset);
    sigprocmask(SIG_BLOCK, &sigset, &oldset);

    while (1)
    {
        memset(&msg, 0x0, sizeof(Record_Message));

        //unblock signals
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGINT);
        sigaddset(&sigset, SIGCHLD);
        sigprocmask(SIG_UNBLOCK, &sigset, &oldset);

        if ( (r = clientSocket.readMessage(msg)) > 0){

            //re-block signals
            sigemptyset(&sigset);
            sigaddset(&sigset, SIGINT);
            sigaddset(&sigset, SIGCHLD);
            sigprocmask(SIG_BLOCK, &sigset, &oldset);

            messageSwitch(msg);
        }
        else if (r == 0){
            printf("%d: Client disconnected.\n", getpid());
            writeLog(6, 0);
            return;
        }
        else{
            continue;
        }
    }

    return;
}



/*!
*	\brief Routes commands to handlers.
*/
void Server::messageSwitch(Record_Message &msg){
    int action = msg.action;

    switch (action){

    case 1: //count
        printf("Received request for Record Count.\n");
        countReply(msg);
        break;
    
    case 2: //Read
        printf("Received Request for Read: %d\n", msg.arg);
        readReply(msg);
        break;

    case 3: //Update
        printf("Received Request for Update: %d\n", msg.arg);
        updateReply(msg);
        break;

    case 4: //Create
        printf("Received Request for Create\n");
        createReply(msg);
        break;

    case 5: //Log
        printf("Received Request for Log\n");
        logReply();
        break;
    default:
        printf("Received unspecified request.\n");
        break;
    }

    //Logs send their own replies
    if (action != 5){
        this->clientSocket.writeMessage(&msg, sizeof(Record_Message));
    }
}



/*!
*	\brief 0's out struct memory.
*/
void Server::composeReply(Record_Message &msg){
    memset(&msg, 0, sizeof(Record_Message));
}



/*!
*	\brief Replies to a count request.
*/
void Server::countReply(Record_Message &msg){
    composeReply(msg);
    msg.action = 1;
    msg.arg = binFile.checkNumRecords();
    writeLog(1, msg.arg);
}



/*!
*	\brief Replies to a create request. 
*/
void Server::createReply(Record_Message &msg){
    Record rec;
    memcpy(&rec, &msg.record, sizeof(Record));
    composeReply(msg);
    msg.action = 4;

    //get number for new month
    int month = binFile.checkNumRecords();
    rec.month = month;

    //append
    if (binFile.writeRecord(rec)){
        msg.arg = month;
        binFile.readRecord(month, msg.record);
    }
    else{
        msg.arg = -1;
    }

    //log
    writeLog(4, msg.arg);
}


/*!
*	\brief Replies to a read request.
*/
void Server::readReply(Record_Message &msg){
    Record record;
    int recNum = msg.arg;
    composeReply(msg);
    msg.action = 2;
    
    //read
    if (binFile.readRecord(recNum, record)){
        memcpy(&msg.record, &record, sizeof(Record));
        msg.arg = recNum;
    }
    else{
        msg.arg = -1;
    }

    //log
    writeLog(2, msg.arg);
}



/*!
*	\brief Replies to an update request
*/
void Server::updateReply(Record_Message &msg){
    int recNum = msg.arg;

    Record rec;
    memcpy(&rec, &msg.record, sizeof(Record));

    composeReply(msg);
    msg.action = 3;

    //check valid record number
    int count = binFile.checkNumRecords();
    if (recNum > count || recNum < 0){
        printf("Invalid record.\n");
        msg.arg = -1;
        return;
    }

    //update
    if (binFile.updateRecord(recNum, rec)){
        msg.arg = recNum;
        binFile.readRecord(recNum, msg.record);
    }
    else{
        msg.arg = -1;
    }

    //log
    writeLog(3, msg.arg);
}



/*!
*	\brief Replies to a log request.
*/
void Server::logReply(){
    int count = logFile.checkNumRecords();

    Log_Message logmsg;

    //loop through all log entries
    for (int i = 0; i < count; i++){
        memset(&logmsg, 0x0, sizeof(Log_Message));
        if (logFile.readRecord(i, logmsg.log)){
            logmsg.arg = 1; //more coming
            //send log
            clientSocket.writeMessage(&logmsg, sizeof(Log_Message));
        }
    }
    logmsg.arg = 0; //done
    clientSocket.writeMessage(&logmsg, sizeof(Log_Message));
}



/*!
*	\brief Logs an operation.
*/
void Server::writeLog(int action, int arg){
    Server_Log_Entry log;

    //prepare log entry
    strcpy(log.ipaddr, clientSocket.getipaddr());
    log.port = clientSocket.getPort();
    log.log.action = action;
    log.log.arg = arg;

    //write
    if (!logFile.writeRecord(log)){
        printf("Error writing to log.\n");
    }
    // else{
    //     printf("Logged.\n");
    // }
}

