/*!	\file SharedMemory.h 
*	\brief  SharedMemory header file.
*   A SharedMemory object encapsulates access to a table of client process information stored in shared memory shared by all clients on the machine. \n
*   The table begins with a single integer field storing the number of clients currently connected on that machine.\n
*   The rest represents a table containing information on each process: its process id, total number of commands issued, connection time, and time of last command.\n
*   The SharedMemory represents a critical section, and accesses are synchronized through the SemaphoreSet.\n
*   
*/

#ifndef SHAREDMEMORY_H 
#define SHAREDMEMORY_H 

#include "Packets.h"
#include <sys/shm.h>
#include "SemaphoreSet.h"
#include <time.h>

#define MAX_CLIENTS 50


/*!
 *	\class SharedMemory
 *	\brief SharedMemory template class
 *  \n
*   A SharedMemory object encapsulates access to a table of client process information stored in shared memory shared by all clients on the machine. \n
*   The table begins with a single integer field storing the number of clients currently connected on that machine.\n
*   The rest represents a table containing information on each process: its process id, total number of commands issued, connection time, and time of last command.\n
*   The SharedMemory represents a critical section, and accesses are synchronized through the SemaphoreSet.\n
 */
class SharedMemory
{
private:

    /*!
    *   \struct Process_Cell
    *   \brief Struct representing a row of the process table.
    */
    struct Process_Cell{
        bool occupied;
        pid_t pid;
        int numCommands;
        time_t startTime;
        time_t lastCmdTime;
    };

    /*!
    *	\var SemaphoreSet sems - Semaphore set object to synchronize access. .
    */
    SemaphoreSet sems;
    /*!
    *	\var pid_t pid
    */
    pid_t pid;

    /*!
    *	\var int shmemid
    */
    int shmemid;
    /*!
    *	\var int* numClientsPtr
    */
    int* numClientsPtr;
    /*!
    *	\var Process_Cell* cellsArray
    */
    Process_Cell* cellsArray;
    /*!
    *	\var int acquiredIndex
    */
    int acquiredIndex;

    /*!
    *   \fn acquireCell
    *	\param none
    *	\brief occupies a cell in the table
    *	\return void
    *   
    *   \par Description
    *   Searches the table for the first unoccupied cell, writes the process's data to it, and sets acquiredCell to that index.
    *
    */    
    void acquireCell();
    /*!
    *   \fn releaseCell
    *	\param none 
    *	\brief Releases the acquired cell
    *	\return none
    *   
    *   \par Description
    *   0's the memory of the acquired table cell.
    *
    */    
    void releaseCell();
    /*!
    *   \fn convertTime
    *	\param time_t t
    *	\brief Converts timestamp to datetime string
    *	\return timestamp string
    *   
    *   \par Description
    *   Converts a time_t timestamp to datetime string
    *
    */    
    const std::string convertTime(time_t t);

public:
    /*!
    *   \fn Constructor
    *	\param pid_t pid : client process id
    *	\param SemaphoreSet sems : SemaphoreSet object representing initialized semaphores.
    *	\brief Constructs a SharedMemory.
    *	\return SharedMemory
    *   
    *   \par Description
    *   Initializes the shared memory if it doesn't already exist. 
    *   Sets pointers to the numclients and cellarray fields.
    *
    */
    SharedMemory(pid_t pid, SemaphoreSet ss);
    /*!
    *   \fn Destructor
    *	\param None.
    *	\brief Destructor. Deallocates the shared memory and semaphores.
    *	\return void
    *   
    *   \par Description
    *   Checks if the closing client in the last client in the system.
    *   If it is, deallocates the shared memory and destroys the semaphores.
    *
    */
    ~SharedMemory();
    /*!
    *   \fn getNumClients
    *	\param none 
    *	\brief Numclients getter
    *	\return Numclients
    *   
    *   \par Description
    *   Returns the contents of the numclients shmem field.
    *
    */
    int getNumClients(){return *numClientsPtr;}
    /*!
    *   \fn getCellIndex
    *	\param none 
    *	\brief AcquiredIndex getter
    *	\return acquiredIndex
    *   
    *   \par Description
    *   Returns the cell index held by this process.
    *
    */
    int getCellIndex(){return acquiredIndex;}
    /*!
    *   \fn printCell
    *	\param int cellIndex
    *	\brief Prints the contents of a table cell 
    *	\return void
    *   
    *   \par Description
    *   Prints the contents of a table cell 
    *
    */
    void printCell(int cellIndex);
    /*!
    *   \fn printConnectedClients
    *	\param none 
    *	\brief prints all occupied table cells.
    *	\return void
    *   
    *   \par Description
    *   Loops through the process table, and prints the contents of any cell whose Process_Cell.occupied is true.
    *
    */
    void printConnectedClients();
    /*!
    *   \fn logCommand
    *	\param none
    *	\brief Logs a command in the table.
    *	\return void
    *   
    *   \par Description
    *   Increments the numcommands field and updates the lastmsgtime field of the acquired table cell.
    *
    */
    void logCommand();
};

#endif