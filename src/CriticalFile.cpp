/*!	\file CriticalFile.cpp
*	\brief  CriticalFile class implementation file.
*/

#include "CriticalFile.h"

template class CriticalFile<Record>;
template class CriticalFile<Server_Log_Entry>;
template class CriticalFile<Client_Log_Entry>;

/*!
*	\brief Constructs a CriticalFile.
*/
template <typename T>
CriticalFile<T>::CriticalFile(const int filedesc, SemaphoreSet ss) : fd(filedesc), sems(ss){}



/*!
*	\brief Destructor. Closes the file.
*/
template <typename T>
CriticalFile<T>::~CriticalFile(){
    // printf("Closing file %d\n", fd);
    close(this->fd);
}



/*!
*	\brief Counts records in the file.
*/
template <typename T>
int CriticalFile<T>::checkNumRecords(){
    int len;

    sems.readerLock();
    if ( (len = lseek(fd, 0, SEEK_END) ) < 0 ){
        perror("Count seek: ");
        sems.readerUnlock();
        return -1;
    }
    else{
        int count = len / sizeof(T); 
        printf("Counted %d\n", count);
        
        sems.readerUnlock();
        return count;
    }
}



/*!
*	\brief Reads a record into the buffer.
*/
template <typename T>
bool CriticalFile<T>::readRecord(const int recordNumber, T &buf){
    sems.readerLock();

    if (!seekRecord(recordNumber)){
        return false;
    }

    int res = read(fd, &buf, sizeof(T));
    if (res <= 0){
        perror("Failed to read from file");
        sems.readerUnlock();
        return false;
    }

    sems.readerUnlock();
    return true;
}



/*!
*	\brief Seeks to record
*/
template <typename T>
bool CriticalFile<T>::seekRecord(const int recordNumber){
    // sems.readerLock();
    if (lseek(fd, recordNumber * sizeof(T), 0) == -1){
        perror("Seek error: ");

        // sems.readerUnlock();
        return false;
    }
    // sems.readerUnlock();
    return true;
}



/*!
*	\brief Appends a record
*/
template <typename T>
bool CriticalFile<T>::writeRecord(T &record){
    sems.writerLock();
    if (lseek(fd, 0, SEEK_END) == -1){
        perror("Create seek:");
        return false;
    }
    
    if (write(fd, &record, sizeof(T)) == -1){
        perror("Create Write:");
        sems.writerUnlock();
        return false;
    }
    else{
        // printf("Wrote new record.\n");
        // msg.arg = checkNumRecords(fd);
        // memcpy(&msg.record, &rec, sizeof(Record));
        sems.writerUnlock();
        return true;

        //the server should call checknum after to get the new number of records.
    }
}



/*!
*	\brief Updates a record.
*/
template <typename T>
bool CriticalFile<T>::updateRecord(const int recordNumber, T &record){
    sems.writerLock();
    if (seekRecord(recordNumber)){
        if (write(fd, &record, sizeof(T)) > 0){
            // printf("Updated record %d.\n", recordNumber);
            sems.writerUnlock();
            return true;
        }
        else{
            perror("Failed to write to file");
        }
    }
    sems.writerUnlock();
    return false;
}
