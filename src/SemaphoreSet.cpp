/*!	\file SemaphoreSet.cpp
*	\brief  SemaphoreSet class implementation file.
*/

#include "SemaphoreSet.h"


/*!
*	\brief Constructs a SemaphoreSet.
*/
SemaphoreSet::SemaphoreSet(/* args */)
{
}


/*!
*	\brief Constructs a SemaphoreSet.
*/
SemaphoreSet::SemaphoreSet(int setID, int setNum) : 
    setID(setID), readCountMutex( setNum * 3 ), readCount(  (setNum * 3) + 1  ), writeSem( (setNum * 3) + 2 )
{}



/*!
*	\brief Destructor.
*/
SemaphoreSet::~SemaphoreSet()
{
}



/*!
*	\brief Gets the semaphore setid from the key
*/
int SemaphoreSet::createSemaphores(int semKey, int numSets){
    int semid;
    int num = numSets * 3;

    //check if exists
    if ( (semid = semget(semKey, num, 0)) == -1 ){
        if (errno != ENOENT){
            perror("semget 1");
            return -1;
        }

        //create
        if ( (semid = semget(semKey, num, 0600|IPC_CREAT)) == -1){
            perror("Semaphore creation failed");
            return -1;
        }

        semun arg;
        int RCMNum;
        //init sems
        for (int i = 0; i < numSets; i++){
            RCMNum = (i * 3);

            if (semctl(semid, RCMNum, SETVAL, 1) == -1){
                perror("Semaphore initialization failed");
                return -1;
            }

            if (semctl(semid, (RCMNum + 1), SETVAL, 0) == -1){
                perror("Semaphore initialization failed");
                return -1;
            }
            if (semctl(semid, (RCMNum + 2), SETVAL, 1) == -1){
                perror("Semaphore initialization failed");
                return -1;
            }
        }

    }
    else{
        //exists
        
        //make sure they've been initalized already
        //acquire one of the mutexes - should only be possible if they're inited
        genericWait(semid, 0);
        genericSignal(semid, 0);

    }

    return semid;

}



/*!
*	\brief Destroys the semaphore set.
*/
void SemaphoreSet::destroySemaphores(){
    if (semctl(setID, 0, IPC_RMID, 0) == -1){
        perror("Failed to remove semaphores");
    }
}



/*!
*	\brief Issues a wait for a reader.
*/
void SemaphoreSet::readerLock(){
    // Reader:
    // P(ReaderCount)
    genericWait(this->setID, this->readCountMutex);

    // #Readers += 1
    genericSignal(this->setID, this->readCount);
    // if (#Readers == 1)
    int semval = getSemValue(readCount);
    if ( semval == 1 ){
        // P(Mutex)
        genericWait(this->setID, this->writeSem);
    }
    // V(ReaderCount)
    genericSignal(this->setID, this->readCountMutex);
}



/*!
*	\brief Issues a signal for a reader.
*/
void SemaphoreSet::readerUnlock(){
    // P(ReaderCount)
    genericWait(this->setID, this->readCountMutex);

    // #Readers -= 1
    genericWait(this->setID, this->readCount);

    int semval = getSemValue(readCount);
    // if (#Readers == 0)
    if ( semval == 0 ){
        // V(Mutex)
        genericSignal(this->setID, this->writeSem);
    }
    // V(ReaderCount)
    genericSignal(this->setID, this->readCountMutex);
}



/*!
*	\brief Waits the mutex
*/
void SemaphoreSet::writerLock(){
    //P(mutex)
    genericWait(this->setID, this->writeSem);
}



/*!
*	\brief Signals the mutex
*/
void SemaphoreSet::writerUnlock(){
    //V(mutex)
    genericSignal(this->setID, this->writeSem);
}



/*!
*	\brief Issues a Wait to a semaphore
*/
void SemaphoreSet::genericWait(int semID, int semNum){
    sembuf wait;
    wait.sem_num = semNum;
    wait.sem_op = -1;
    wait.sem_flg = 0;
    if (semop(semID, &wait, 1) == -1){
        perror("wait semop");
        return;
    }
}



/*!
*	\brief Issues a signal to a semaphore
*/
void SemaphoreSet::genericSignal(int semID, int semNum){
    struct sembuf signal = {(ushort)semNum, 1, 0};
    if (semop(semID, &signal, 1) == -1){
        perror("signal semop");
        return;
    }

}



/*!
*	\brief Prints the semaphores' values
*/
void SemaphoreSet::printSemValues(){
    printf("%d RCM: %d | RC: %d | W: %d\n", getpid(), getSemValue(this->readCountMutex), 
        getSemValue(this->readCount), getSemValue(this->writeSem)
    );
}



/*!
*	\brief Gets semaphore value.
*/
int SemaphoreSet::getSemValue(int semNum){
    semun arg;
    int semValue = semctl(setID, semNum, GETVAL, arg);
    // //printf("%d value: %d\n", semNum, semValue);
    return semValue;
}