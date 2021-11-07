/*!
\mainpage 552 Project 3 - Distributed Data Server
\n
This application implements a distributed data server that communicates with clients through sockets.\n 
The server application performs operations requested by the client on a binary data file. The operations supported are shown below.\n

When the server received a connection through its socket, it forks a child process to service that client. 

Both the server and clients keep a log file of all actions performed. These log files are shared between all server child processes and all client processes on a machine, respectively. \n

Client processes on a machine also share access to shared memory on that machine. This shared memory implements a table containing data on all connected clients.\n
The table begins with a single integer field storing the number of clients currently connected on that machine.\n
The rest represents a table containing information on each process: its process id, total number of commands issued, connection time, and time of last command.\n
This table is updated by the client every time it issues a command to the server.

Several pieces of data are shared between multiple processes, each representing a critical section. To guard them, both the servers and clients allocate sets of semaphores on startup that they use to synchronize access according to the readers-writers algorithm. 

Client Commands:\n
 - D)isplay Record          : Read and display a single record from the data file. Entering '-999' displays all records. \n
 - C)hange Record           : Update a record with new values. \n
 - N)ew Record              : Add a new record to the file. \n
 - S)how Server Log         : List the contents of the server's log file. \n
 - L)Show Client Log        : List the contents of the client machine's log file. \n
 - P)Show Connected Clients : List the contents of the client machine's process table. \n 
 - X)Exit                   : Exits the client. \n


This application uses data representing percentage of market shares of mobile operating systems from January 2020 to January 2021.
Data was taken from this source: https://www-statista-com.eu1.proxy.openathens.net/statistics/272698/global-market-share-held-by-mobile-operating-systems-since-2009/
It was cropped from January 2020 onward, and redundant fields were removed.

*/

/*!	\file p3ser.cpp 
*	\brief  Server program for a data server application.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: p3ser.cpp\n
*   \b Purpose: Process commands from a client program using sockets.\n
*   \n
*   This application will perform operations on binary data received from a client program through a socket. 
*   Child processes are spawned to service individual clients.
*   Operations on the binary data file and the log file are guarded by semaphores that are initialized on server startup if not present.
*   Signals are used to track terminating child servers. 
*
*/

#include <sys/wait.h>

#include "Server.h"

#define PORT 15006

int numClients = 0;
int semid = 0;
int socketfd = 0;
int logfd = 0;
int binfd = 0;

bool quickExit = false;

/*!
*   \fn sigchldHandler
*	\param int signum: 
*	\brief Sigint handler
*	\return 
*   
*   \par Description
*   Sigint handler. Asks user if it should close the server. If it does, it destroys the semaphores. 
*
*/
void sigintHandler(int signum);
/*!
*   \fn sigchldHandler
*	\param int signum: 
*	\brief Sigchld handler
*	\return
*   
*   \par Description
*   Sigchld handler. Decrements numclients. If numclients = 0, sends a sigint to the server.
*
*/
void sigchldHandler(int signum);

/*!
*   \fn main
*	\param int argc: 
*	\param char const *argv[]: 
*	\brief Main routine
*	\return int
*   
*   \par Description
*   Creates the socket, awaits connections, and spawns child data servers.
*
*/
int main(int argc, char const *argv[]){

    if (argc > 1){
        if (strcmp(argv[1], "q") == 0){
            quickExit = true;
        }
    }



    sockaddr_in serverAddress, clientAddress;
    socklen_t sockAddrLength = sizeof(sockaddr_in);
    int pid, clientfd;

    //Register handlers
    signal(SIGINT, sigintHandler);
    signal(SIGCHLD, sigchldHandler);

    //init semaphores
    semid = SemaphoreSet::createSemaphores(PORT, 2);
    if (semid == -1){
        printf("Failed to create semaphores.\n");
        exit(3);
    }

    //open bin file
    binfd = open("data/out.bin", O_RDWR);
    if (binfd == -1){
        perror("Failed to open binary file");
        exit(0);
    }

    //open log file
    logfd = open("logs/log.ser", O_CREAT | O_RDWR, 0600);
    if (logfd == -1){
        perror("Failed to open log file");
        exit(1);
    }

    //open socket
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0){ 
        perror("Socket:"); 
        exit(2); 
    }

    //set socket to reuse address
    int reuse = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    //bind socket
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketfd, (sockaddr*) &serverAddress, sizeof(serverAddress)) < 0){ 
        perror("Bind:");
        exit(3);
    }

    //listen on socket
    if (listen(socketfd, 5) == -1){
        perror("Listen:");
        exit(4);
    } 

    //block all signals
    sigset_t sigset, oldset;
    sigfillset(&sigset);
    sigprocmask(SIG_BLOCK, &sigset, &oldset);

    //accept connections
    while (1){
        memset(&clientAddress, 0x0, sizeof(sockaddr_in));

        //unblock signals

        sigemptyset(&sigset);
        sigaddset(&sigset, SIGINT);
        sigaddset(&sigset, SIGCHLD);
        sigprocmask(SIG_UNBLOCK, &sigset, &oldset);


        /*If a signal interrupts accept, it won't reenter automatically. 
        It'll continue down and fork a useless process.
        */
        printf("Awaiting connection.\n");
        if ( (clientfd = accept(socketfd, (sockaddr*) &clientAddress, &sockAddrLength)) < 0){
            perror("Accept:");
            continue;
        }

        //re-block signals
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGINT);
        sigaddset(&sigset, SIGCHLD);
        sigprocmask(SIG_BLOCK, &sigset, &oldset);

        pid = fork();
        if (pid == -1){ //error
            perror("Fork:");
            continue;
        }
        else if (pid == 0){ //child
            printf("Client connected: %s : %d\n", inet_ntoa(clientAddress.sin_addr), (int) ntohs(clientAddress.sin_port));

            //exit won't call destructors before terminating the process.
            //returning won't send sigchld
            //new'ing so I can delete to force the destructors to run before exiting. 
            Server* server = new Server(binfd, clientfd, logfd, clientAddress, semid);
            server->run();
            delete server;
            exit(0);
        }
        else{ //parent
            //increment numclients
            numClients++;            
        }


    }
    

    close(binfd);
    close(logfd);
    close(socketfd);

    return 0;
}



void sigintHandler(int signum){
    //ask first

    if (numClients > 0){
        return;
    }

    char field;
    while (!quickExit){
        printf("Shutdown server? [y/n]\n>>>");
        fflush(stdout);

        while( (field = getchar()) == '\n');

        switch (toupper(field)){
        case 'N':
            printf("Continuing.\n");
            return;
        case 'Y':
            quickExit = true;
            break;
        default:
            printf("Invalid\n");
            continue;
        }
    }

    if (semctl(semid, 0, IPC_RMID, 0) == -1){
        perror("Failed to remove semaphores");
    }
    // else{
    //     printf("Semaphores destroyed.\n");
    // }

    close(socketfd);
    close(logfd);
    close(binfd);

    printf("\nServer shut down.\n");

    exit(0);


}

void sigchldHandler(int signum){
    printf("Child shut down.\n");
    numClients--;
    if (numClients == 0){
        kill(getpid(), SIGINT);
    }
}
