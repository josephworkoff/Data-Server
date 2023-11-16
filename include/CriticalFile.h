/*!	\file CriticalFile.h 
*	\brief  Define a wrapper class for a file representing a critical section.
*   A CriticalFile object represents an open file which is read from and written to concurrently, conferring a critical section. \n
*   The file is a binary file whose contents are structured using the struct type used to construct the object. \n
*   A CriticalFile object supports read, update, append, and record count operations on the file. \n
*   Accesses are synchronized through its SemaphoreSet member. \n
*   
*/

#ifndef CRITICALFILE_H 
#define CRITICALFILE_H   

#include "Packets.h"
#include "SemaphoreSet.h"

/*!
 *	\class CriticalFile
 *	\brief CriticalFile template class
 *  \n
*   A CriticalFile object represents an open file which is read from and written to concurrently, conferring a critical section. \n
*   The file is a binary file whose contents are structured using the struct type used to construct the object. \n
*   A CriticalFile object supports read, update, append, and record count operations on the file. \n
*   Accesses are synchronized through its SemaphoreSet member. \n
 */
template<typename T>
class CriticalFile
{
private:
    /*!
    *	\var const int fd - Open file descriptor.
    */
    const int fd;
    /*!
    *	\var SemaphoreSet sems - Semaphore set object to synchronize access.
    */
    SemaphoreSet sems;

    /*!
    *   \fn seekRecord
    *	\param const int recordNumber : record to seek to
    *	\brief Seeks to record
    *	\return false on error.
    *   
    *   \par Description
    *   Moves file pointer to specified record.
    *   Operation is NOT synched. 
    */
    bool seekRecord(const int recordNumber);

public:
    /*!
    *   \fn Constructor
    *	\param const int filedesc : Open file descriptor.
    *	\param SemaphoreSet sems : SemaphoreSet object representing initialized semaphores.
    *	\brief Constructs a CriticalFile.
    *	\return CriticalFile
    *   
    *   \par Description
    *   Sets the fd and sems members.
    *
    */
    CriticalFile(const int filedesc, SemaphoreSet sems);
    /*!
    *   \fn Destructor
    *	\param None.
    *	\brief Destructor. Closes the file.
    *	\return void
    *   
    *   \par Description
    *   Calls close() on the file descriptor. Does NOT deallocate system semaphores.
    *
    */
    ~CriticalFile();
    /*!
    *   \fn readRecord
    *	\param const int recordNumber : Record number to read
    *	\param T &buf : Buffer to read record into.
    *	\brief Reads a record into the buffer.
    *	\return false on error, true otherwise.
    *   
    *   \par Description
    *   Reads the specified record into the template type buffer.
    *   Operation is read-synched.
    *
    */
    bool readRecord(const int recordNumber, T &buf);
    /*!
    *   \fn writeRecord
    *	\param T &record : Record to append
    *	\brief Appends a record
    *	\return false on error
    *   
    *   \par Description
    *   Appends a template type record onto the file. 
    *   Operation is write-synched.
    *
    */
    bool writeRecord(T &record);
    /*!
    *   \fn updateRecord
    *	\param const int recordNumber : record to update 
    *	\param T &record : new record information
    *	\brief Updates a record.
    *	\return false on error.
    *   
    *   \par Description
    *   Overwrites the specified record with the new template type data.
    *   Operation is write-synched.
    *
    */
    bool updateRecord(const int recordNumber, T &record);
    /*!
    *   \fn checkNumRecords
    *	\param none 
    *	\brief Counts records in file.
    *	\return Number of records, or -1 on error
    *   
    *   \par Description
    *   Counts the number of template type structs of data are in the file.
    *
    */
    int checkNumRecords();

};

#endif