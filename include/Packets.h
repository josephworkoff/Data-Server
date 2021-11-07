/*!	\file Packets.h
*	\brief  Header file for packet structs.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: Packets.h\n
*   \b Purpose: Struct definitions for message packets.\n
*   \n
*   This file defines several common structs to be used to send and receive data through the socket. 
*/

#ifndef PACKETS_H 
#define PACKETS_H   

#include <stdio.h>
#include <sys/ipc.h>
// #include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>


#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <arpa/inet.h>
#include <sys/sem.h>
#include <errno.h>

/*!
*   \struct Record
*   \brief Struct for holding data from a single record.
*/
struct Record{
    int month;
    float android;
    float ios;
    float kaios;
    float other;
};

/*!
*   \struct Record_Message
*   \brief Generic packet struct for transfering operation commands and record data.
*/
struct Record_Message{
    int action;
    int arg;
    Record record;
};

/*!
*   \struct Log
*   \brief Represents a logged action.
*/
struct Log{
    int action;
    int arg;

    /*!
    *	\fn print
    *	Decodes a log action.
    */
    void print(){
        
        switch (action)
        {
        case 1: //count
            printf("Requested Record Count (%d).\n", arg);
            break;
        case 2: //read
            printf("Read Record %d.\n", arg);
            break;
        case 3: //Update
            printf("Updated Record %d.\n", arg);
            break;
        case 4: //Created
            printf("Created Record %d.\n", arg);
            break;
        case 5: //Connected
            printf("Connected to server.\n");
            break;
        case 6: //Disonnected
            printf("Disconnected from server.\n");
            break;
        default:
            printf("Performed unspecified action (%d|%d).\n", action, arg);
            break;
        }
    }
};

/*!
*   \struct Server_Log_Entry
*   \brief Server log file entry. Stores client address info.
*/
struct Server_Log_Entry{
    char ipaddr[INET_ADDRSTRLEN];
    int port;
    Log log;

    void print(){
        printf("%s:%d | ", ipaddr, port);
        log.print();
    }
};

/*!
*   \struct Log_Message
*   \brief Struct for transfering a single server log file entry.
*/
struct Log_Message{
    int arg;
    Server_Log_Entry log;
};

/*!
*   \struct Client_Log_Entry
*   \brief Client log file entry. Stores client process address info.
*/
struct Client_Log_Entry{
    pid_t pid; 
    Log log;
    void print(){
        printf("Process %d | ", pid);
        log.print();
    }
};

#endif