/*!	\file SocketConnection.cpp
*	\brief  SocketConnection class implementation file.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: SocketConnection.cpp\n
*   \b Purpose: Implement the SocketConnection class.\n
*   \n
*
*/


#include "SocketConnection.h"

/*!
*	\brief Constructs a SocketConnection.
*/
SocketConnection::SocketConnection(const int sockfd, const sockaddr_in addr) :
    socketfd(sockfd), address(addr) {}



/*!
*	\brief Destructor. Closes socket descriptor.
*/
SocketConnection::~SocketConnection() {
    close(socketfd);
}



/*!
*	\brief Reads a message from the socket.
*/
int SocketConnection::readMessage(Record_Message &msg){
    memset(&msg, 0, sizeof(Record_Message));
    // printf("Reading.\n");
    int r; 
    if ( (r = read(socketfd, &msg, sizeof(Record_Message) ) ) < 0){
        perror("Read:");
        return 0;
    }
    else{
        // printf("Read %d.\n", r);
        // printf("msg.arg = %d\n", msg.arg);
        return r;
    }
}



/*!
*	\brief Generic read.
*/
int SocketConnection::readMessage(void* msg, int size){
    // Record_Message msg;
    memset(msg, 0, size);
    // printf("Reading.\n");
    int r; 
    if ( (r = read(socketfd, msg, size ) ) < 0){
        perror("Read:");
        return 0;
    }
    else{
        // printf("Read %d.\n", r);
        return r;
    }
}



/*!
*	\brief Writes a message to the socket.
*/
int SocketConnection::writeMessage(Record_Message &msg){
    // printf("Writing.\n");
    int w;
    if ( (w = write(socketfd, &msg, sizeof(Record_Message) ) ) < 0){
        perror("Write:");
        return 0;
    }
    else{
        // printf("Wrote: %d\n", w);
        // printf("msg.arg = %d\n", msg.arg);
        return w;
    }
}



/*!
*	\brief Generic write.
*/
int SocketConnection::writeMessage(void* msg, int size){
    // printf("Writing.\n");
    int w;
    if ( (w = write(socketfd, msg, size ) ) < 0){
        perror("Write:");
        return 0;
    }
    else{
        // printf("Wrote: %d\n", w);
        return w;
    }
}



/*!
*	\brief Socket descriptor getter.
*/
int SocketConnection::getSocketfd(){return this->socketfd;}



/*!
*	\brief Port getter.
*/
int SocketConnection::getPort(){return (int) ntohs(address.sin_port);}



/*!
*	\brief IP address getter.
*/
const char* SocketConnection::getipaddr(){return inet_ntoa(address.sin_addr);}