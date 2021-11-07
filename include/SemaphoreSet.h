/*!	\file SemaphoreSet.h 
*	\brief  SemaphoreSet header file.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: SemaphoreSet.h\n
*   \b Purpose: Define a wrapper class for a set of semaphores synchronizing read and write operations on critical sections.\n
*   \n
*   A SemaphoreSet object represents a set of 3 pre-initialized system semaphores used to synchronize read and write operations on critical sections.\n
*   It uses the 3 semaphores to implement the Readers-Writers algorithm, where the first represents the ReaderCount mutex, the second counts the number of readers, and the third represents the write mutex.\n
*   
*/

#ifndef SEMAPHORESET_H 
#define SEMAPHORESET_H 

#include "Packets.h"

/*!
 *	\class SemaphoreSet
 *	\brief SemaphoreSet wrapper class
 *  \n
 *   A SemaphoreSet object represents a set of 3 pre-initialized system semaphores used to synchronize read and write operations on critical sections.\n
 *   It uses the 3 semaphores to implement the Readers-Writers algorithm, where the first represents the ReaderCount mutex, the second counts the number of readers, and the third represents the write mutex.\n
 *   
 */
class SemaphoreSet
{
private:
    /*!
    *	\var int setID.
    */
    int setID;
    /*!
    *	\var int readCountMutex.
    */
    int readCountMutex;
    /*!
    *	\var int readCount.
    */
    int readCount;
    /*!
    *	\var int writeSem.
    */
    int writeSem;

    /*!
    *   \union semun
    *   \brief Union for semop.
    */
    union semun {
        int val;
        struct semid_ds* buf;
        unsigned short  *array;
        #if defined(_linux_)
            struct seminfo  *__buf;
        #endif
    };

    /*!
    *   \fn getSemValue
    *	\param int semNum: semaphore number.
    *	\brief Gets semaphore value.
    *	\return value of semaphore
    *   
    *   \par Description
    *   Gets the value of the semNumth semaphore in the set.
    *
    */
    int getSemValue(int semNum);

public:
    /*!
    *   \fn Constructor
    *	\param none.
    *	\brief Constructs a SemaphoreSet.
    *	\return SemaphoreSet
    *   
    *   \par Description
    *   Default constructor.
    *
    */
    SemaphoreSet(/* args */);
    /*!
    *   \fn Constructor
    *	\param none.
    *	\brief Constructs a SemaphoreSet.
    *	\return SemaphoreSet
    *   
    *   \par Description
    *   Sets the semaphore setid and semaphore numbers.
    *
    */
    SemaphoreSet(int setID, int setNum);
    /*!
    *   \fn Destructor
    *	\param None.
    *	\brief Destructor.
    *	\return void
    *   
    *   \par Description
    *   Default Destructor.
    *
    */
    ~SemaphoreSet();
    /*!
    *   \fn createSemaphores
    *	\param int semKey: system semaphore key 
    *	\param int numSets: number of sets of 3 semaphores to create
    *	\brief Gets the semaphore setid
    *	\return semaphore set id
    *   
    *   \par Description
    *   Returns the set id of the semaphores on the system with the same key. 
    *   If they do not exist, allocates them, and initializes them to 1 0 1.
    *
    */
    static int createSemaphores(int semKey, int numSets);
    /*!
    *   \fn readerLock
    *	\param none
    *	\brief Issues a wait for a reader.
    *	\return void
    *   
    *   \par Description
    *   Performs the pre-critical section wait of the reader algorithm.
    *
    */
    void readerLock();
    /*!
    *   \fn readerUnock
    *	\param none
    *	\brief Issues a signal for a reader.
    *	\return void
    *   
    *   \par Description
    *   Performs the post-critical section signal of the reader algorithm.
    *
    */
    void readerUnlock();
    /*!
    *   \fn writerLock
    *	\param : none
    *	\brief Waits the mutex
    *	\return void
    *   
    *   \par Description
    *   Issues a wait on the write mutex.
    *
    */
    void writerLock();
    /*!
    *   \fn writerUnlock
    *	\param : none
    *	\brief Signals the mutex
    *	\return void
    *   
    *   \par Description
    *   Issues a signal on the write mutex.
    *
    */
    void writerUnlock();
    /*!
    *   \fn printSemValues
    *	\param : none
    *	\brief Prints the semaphores' values
    *	\return void
    *   
    *   \par Description
    *   Prints the values of all semaphores in this set.
    *
    */
    void printSemValues();
    /*!
    *   \fn destroySemaphores
    *	\param none
    *	\brief Destroys the semaphore set.
    *	\return void
    *   
    *   \par Description
    *   Deallocates the semaphores in this set.
    *
    */
    void destroySemaphores();
    /*!
    *   \fn genericWait
    *	\param int semID: 
    *	\param int semNum: 
    *	\brief Issues a Wait to a semaphore
    *	\return void
    *   
    *   \par Description
    *   Decrements the value of the semNumth semaphore in the semid set, blocking if necessary. 
    *
    */
    static void genericWait(int semID, int semNum);
    /*!
    *   \fn genericSignal
    *	\param int semID: 
    *	\param int semNum: 
    *	\brief Issues a signal to a semaphore
    *	\return void
    *   
    *   \par Description
    *   Increments the value of the semNumth semaphore in the semid set. 
    *
    */
    static void genericSignal(int semID, int semNum);
};

#endif