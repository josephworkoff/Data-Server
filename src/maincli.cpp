/*!	\file p3cli.cpp 
*	\brief  Client program for a data server application.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: p3cli.cpp\n
*   \b Purpose: Send commands to a server program using sockets.\n
*   \n
*   This application will connect to a server program through a socket. 
*   Once connected, it presents a user menu, and sends user-input requests to the server to perform read and write operations on the server's data file. 
*   All operations are logged in a machine-specific log file. 
*   On startup, the client will initialize system shared memory on its machine if it is not present. All client programs on that machine
*   read and write to a table of client process data in that shared memory.
*   On startup, the client will also initialize system semaphores if they are not present, and uses them to protect the shared data.
*   The client blocks all signals for its lifetime, and must be exited through its menu option. 
*
*/

#include "Client.h"

#define SERVER_ADDR "127.0.0.1"
#define PORT 15006



/*!
*   \fn main
*	\param int argc: 
*	\param char const *argv[]: 
*	\brief Main routine
*	\return int
*   
*   \par Description
*   Connects to server and creates the client.
*
*/
int main(int argc, char const *argv[]){
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
        
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    //block all signals
    sigset_t sigset, oldset;
    sigfillset(&sigset);
    sigprocmask(SIG_BLOCK, &sigset, &oldset);

    //open log file

    char hostbuf[256];
    gethostname(hostbuf, 256);

    char logbuf[256];
    sprintf(logbuf, "logs/log-%s.cli", hostbuf);

    const int logfd = open(logbuf, O_CREAT | O_RDWR, 0600);
    if (logfd == -1){
        perror("Failed to open log file");
        exit(1);
    }


    const int socketfd = socket(AF_INET, SOCK_STREAM, 0); //create socket
    if (socketfd == -1){
        perror("Socket: ");
        exit(1);
    }
    
    socklen_t sockAddrLength = sizeof(sockaddr_in);

    if (connect(socketfd, (sockaddr *)&serverAddress, sockAddrLength) == -1){ //connect endpoints

        if (errno == ECONNREFUSED){
            printf("Server is not online.\n");
            
        }
        else{
            perror("Connect: ");
        }

    }
    else{
        // printf("Connected.\n");
        // write(socketfd);


        int semid = SemaphoreSet::createSemaphores(getuid(), 2);
        // printf("semid=%d\n", semid);

        Client *client = new Client(socketfd, logfd, serverAddress, semid);
        client->run();
        delete client;
    }

    close(logfd);
    close(socketfd);

    return 0;
}
