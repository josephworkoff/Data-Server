/*!	\file SharedMemory.cpp
*	\brief  SharedMemory class implementation file.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: SharedMemory.cpp\n
*   \b Purpose: Implement the SharedMemory class.\n
*   \n
*
*/

#include "SharedMemory.h"

/*!
*	\brief Constructs a SharedMemory.
*/
SharedMemory::SharedMemory(pid_t pid, SemaphoreSet ss) : 
    pid(pid), sems(ss)
{

    bool exists;

    size_t memsize = sizeof(int) + (sizeof(Process_Cell) * MAX_CLIENTS);

    //check if shmem already exists
    if ( (shmemid = shmget(getuid(), memsize, 0600|IPC_CREAT|IPC_EXCL) ) == -1 ){

        if (errno != EEXIST){
            perror("shmget IPC_CREAT|IPC_EXCL");
            exit(1);
        }

        //Exists
        if ( (shmemid = shmget(getuid(), memsize, 0600) ) == -1 ){
            perror("shmget");
            exit(1);
        }

        exists = true;

        // printf("Using existing shmem.\n");
    }
    
    else{ //Created - init
        // printf("Created new shmem.\n");
        exists = false;
        //lock here
        sems.writerLock();
    }

    //First sizeof(int) bytes in the shmem is the numclients field.
    numClientsPtr = (int*)shmat(shmemid, 0, 0);
    //The rest are client data cells
    cellsArray = (Process_Cell*) (numClientsPtr + 1);
    
    //Initialize
    if (!exists){
        new(cellsArray) Process_Cell[MAX_CLIENTS];
        memset( (void*)numClientsPtr, 0x0, memsize);
        //unlock
        sems.writerUnlock();
    }

    acquireCell();
}

/*!
*	\brief Destructor. Deallocates the shared memory and semaphores.
*/
SharedMemory::~SharedMemory(){

    // printf("~shmem\n");

    sems.writerLock();

    releaseCell();

    *numClientsPtr = (*numClientsPtr) - 1;

    if (*numClientsPtr <= 0){

        // printf("Final client disconnected. Destroying IPC.\n");

        shmdt((void*)numClientsPtr);
        shmctl(shmemid,IPC_RMID,0);
        sems.destroySemaphores();

        //this will break if a new client has connected in the meantime.
    }
    else{
        sems.writerUnlock();
    }
}



/*!
*	\brief occupies a cell in the table
*/
void SharedMemory::acquireCell(){

    acquiredIndex = -1;
    sems.writerLock();

    for (int i = 0; i < MAX_CLIENTS; i++){
        if (!cellsArray[i].occupied){
            cellsArray[i].occupied = true;
            cellsArray[i].pid = pid;
            cellsArray[i].numCommands = 0;

            time_t t;
            time(&t);
            cellsArray[i].startTime = t;
            cellsArray[i].lastCmdTime = t;

            acquiredIndex = i;

            *numClientsPtr = (*numClientsPtr) + 1;

            // printf("Acquired shmem cell %d\n", acquiredIndex);

            break;
        }
    }

    sems.writerUnlock();
    if (acquiredIndex == -1){
        //Failed to find a slot
        printf("Max client capacity reached. Try again later.\n");
        exit(0);
    }
}


/*!
*	\brief prints all occupied table cells.
*/
void SharedMemory::printConnectedClients(){
    sems.readerLock();

    printf("%d client(s) connected.\n", *numClientsPtr);
    printf("\n%5s |#CMDS|      Start Time     |   Last CMD Time\n", "PID");

    //print all occupied cells
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (cellsArray[i].occupied == true){
            printCell(i);
        }
    }

    sems.readerUnlock();
}



/*!
*	\brief Prints the contents of a table cell 
*/
void SharedMemory::printCell(int cellIndex){
    // sems.readerLock();
    Process_Cell cell = cellsArray[cellIndex];
    printf("%d | %3d | %s | %s\n", cell.pid, cell.numCommands, convertTime(cell.startTime).c_str(), convertTime(cell.lastCmdTime).c_str() );
    // sems.readerUnlock();
}



/*!
*	\brief Releases the acquired cell
*/
void SharedMemory::releaseCell(){
    memset( &(cellsArray[acquiredIndex]), 0x0, sizeof(Process_Cell));
}


/*!
*	\brief Converts timestamp to datetime string
*/
const std::string SharedMemory::convertTime(time_t t){
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}


/*!
*	\brief Logs a command in the table.
*/
void SharedMemory::logCommand(){
    sems.writerLock();

    cellsArray[acquiredIndex].numCommands = (cellsArray[acquiredIndex].numCommands + 1);

    time_t t;
    time(&t);
    cellsArray[acquiredIndex].lastCmdTime = t;

    sems.writerUnlock();
}