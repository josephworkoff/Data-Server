/*!	\file Client.h 
*	\brief Client class header file.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: Client.h\n
*   \b Purpose: Define a client program class.\n
*   \n
*   A Client object represents a connected client process. \n
*   It handles all communication with a single data server. \n
*   The operation lifetime of a Client is its run method. \n
*   Operations on the log file are handled in the CriticalFile<Client_Log_Entry>.\n
*   Communication with the server is performed in SocketConnection serverSocket.\n
*   Operations on the shared memory on the client machine are handled in the SharedMemory shmem.\n
*   Wherever numeric codes are used to correspond to operations, these codes are used:\n
*      Num : Operation\n
*        1 : Count\n
*        2 : Read\n
*        3 : Update\n
*        4 : Create\n
*        5 : (Log when sending a request to the server, Client Connection when logging actions.)\n
*        6 : Client disconnection\n
*   
*/

#ifndef CLIENT_H 
#define CLIENT_H  

#include "SocketConnection.h"
#include "CriticalFile.h"
#include "SharedMemory.h"
#include "SemaphoreSet.h"
#include <vector>



/*!
 *	\class Client
 *	\brief Client process class
 *  \n
*   A Client object represents a connected client process. \n
*   It handles all communication with a single data server. \n
*   The operation lifetime of a Client is its run method. \n
*   Operations on the log file are handled in the CriticalFile<Client_Log_Entry>.\n
*   Communication with the server is performed in SocketConnection serverSocket.\n
*   Operations on the shared memory on the client machine are handled in the SharedMemory shmem.\n
 */
class Client
{
private:
    /*!
    *	\var const pid_t pid : Client process ID.
    */
    const pid_t pid;
    /*!
    *	\var SocketConnection serverSocket - Handles communication with server.
    */
    SocketConnection serverSocket;
    /*!
    *	\var CriticalFile<Client_Log_Entry> logFile - Performs accesses and operations on the log file..
    */
    CriticalFile<Client_Log_Entry> logFile;
    /*!
    *	\var SharedMemory shmem - Performs accesses and operations on the client machine's shared memory.
    */
    SharedMemory shmem;

    /*!
    *   \fn menuSwitch
    *	\param char choice: Character entered by user.
    *	\brief Routes user input to a handler function.
    *	\return False if exit selected, true otherwise.
    *   
    *   \par Description
    *   Calls the appropriate operation handler function based on the menu option selected by the user.
    *
    */
    bool menuSwitch(char choice);
    /*!
    *   \fn messageSwitch
    *	\param Record_Message &msg: Message received from server.
    *	\brief Routes server reply to a handler function.
    *	\return void
    *   
    *   \par Description
    *   Calls the appropriate operation handler function based on the action field of the reply received from the server.
    *
    */
    void messageSwitch(Record_Message &msg);
    /*!
    *   \fn requestCount
    *	\param : 
    *	\brief 
    *	\return
    *   
    *   \par Description
    *   
    *
    */
    int requestCount();
    /*!
    *   \fn readMenu
    *	\param none
    *	\brief Gets user input for a read operation
    *	\return void
    *   
    *   \par Description
    *   Requests the record count from the server. 
    *   Prompts user for a valid record number to read. 
    *   Calls requestRead.
    *
    */
    void readMenu();
    /*!
    *   \fn requestRead
    *	\param const int recordNumber: record number to request
    *	\brief Requests a record from the server.
    *	\return true if successfully sent.
    *   
    *   \par Description
    *   Calls serverSocket.writeMessage to sends a read request.
    *   msg.action = 2. msg.arg = record number
    *
    */
    bool requestRead(const int recordNumber);
    /*!
    *   \fn receiveRead
    *	\param Record_Message &msg: message received from server
    *	\brief Receives a reply to a read request
    *	\return void
    *   
    *   \par Description
    *   Prints the record received from the server following a read operation.
    *   Logs the read operation. 
    *
    */
    void receiveRead(Record_Message &msg);
    /*!
    void readAll(const int count);
    *   \fn 
    *	\param const int count: record count received from the server
    *	\brief Requests all records from the server
    *	\return void
    *   
    *   \par Description
    *   Repeatedly sends read requests to the server. 
    *   Replies are immediately read. 
    *   Logs each individual read request.
    *   Prints all records received.
    *
    */
    void readAll(const int count);
    /*!
    *   \fn printRecords
    *	\param std::vector<Record> &records: records to print
    *	\brief Prints a number of records
    *	\return void
    *   
    *   \par Description
    *   Prints all records contained in a vector in a table format.
    *
    */
    void printRecords(std::vector<Record> &records);
    /*!
    *   \fn getInt
    *	\param none 
    *	\brief Gets an int from the user
    *	\return Input integer
    *   
    *   \par Description
    *   Takes an integer through stdin. Repeatedly calls until a valid integer is read.
    *
    */
    int getInt();
    /*!
    *   \fn getFloat
    *	\param none
    *	\brief Gets a float from the user
    *	\return Input float
    *   
    *   \par Description
    *   Takes a float through stdin. Repeatedly calls until a valid float is read.
    *
    */
    float getFloat();
    /*!
    *   \fn getRecord
    *	\param Record &rec: record struct to store new record
    *	\brief Creates a new record from user input
    *	\return void
    *   
    *   \par Description
    *   Prompts the user to enter values for each field in a record.
    *
    */
    void getRecord(Record &rec);
    /*!
    *   \fn updateMenu
    *	\param none
    *	\brief Gets user input for an update operation
    *	\return void
    *   
    *   \par Description
    *   Requests the record count from the server. 
    *   Prompts user for a valid record number to update. 
    *   Requests that record, and reads immediately.
    *   Prompts the user to select and enter values for any record field.
    *   Calls requestUpdate when confirmed.
    *
    */
    void updateMenu();
    /*!
    *   \fn requestUpdate
    *	\param const int recordNumber: record number to update
    *	\param const Record &rec: struct containing updated record information
    *	\brief Requests an update from the server.
    *	\return true if successfully sent.
    *   
    *   \par Description
    *   Calls serverSocket.writeMessage to send an update request.
    *   msg.action = 3. msg.arg = record number, msg.record contains record information.
    *
    */
    bool requestUpdate(const int recordNumber, const Record &rec);
    /*!
    *   \fn receiveUpdate
    *	\param Record_Message &msg: message received from server
    *	\brief Receives a reply to an update request
    *	\return void
    *   
    *   \par Description
    *   Prints the record updated by the server following an update operation.
    *   Logs the update operation. 
    *
    */
    void receiveUpdate(Record_Message &msg);
    /*!
    *   \fn createMenu
    *	\param none
    *	\brief Gets user input for a create operation
    *	\return void
    *   
    *   \par Description
    *   Calls getRecord to get new record information from the user.
    *   Calls requestCreate.
    *
    */
    void createMenu();
    /*!
    *   \fn requestCreate
    *	\param const int recordNumber: record number to update
    *	\brief Requests a create from the server.
    *	\return true if successfully sent.
    *   
    *   \par Description
    *   Calls serverSocket.writeMessage to send a create request.
    *   msg.action = 4, msg.record contains record information.
    *
    */
    bool requestCreate(const Record &rec);
    /*!
    *   \fn receiveCreate
    *	\param Record_Message &msg: message received from server
    *	\brief Receives a reply to a create request
    *	\return void
    *   
    *   \par Description
    *   Prints the record created by the server following an created operation.
    *   Logs the create operation. 
    *
    */
    void receiveCreate(Record_Message &msg);
    /*!
    *   \fn requestLog
    *	\param none
    *	\brief Requests the log file entries from the server
    *	\return true if successfully sent.
    *   
    *   \par Description
    *   Calls serverSocket.writeMessage to send a log request. Calls receiveLogs to immediate read.
    *   msg.action = 5.
    */
    bool requestLog();
    /*!
    *   \fn receiveLog
    *	\param none 
    *	\brief Receives all log entries from server.
    *	\return void
    *   
    *   \par Description
    *   Repeatedly calls serverSocket.readMessage to receive log entries from the server as long as msg.arg > 0.
    *   Prints contents of all log entries received. 
    * 
    */
    void receiveLog();
    /*!
    *   \fn clientLog
    *	\param none
    *	\brief Prints client machine's log file
    *	\return void
    *   
    *   \par Description
    *   Reads and prints the contents of the client machine's log file. 
    *
    */
    void clientLog();
    /*!
    *   \fn printLogs
    *	\param std::vector<Server_Log_Entry> &logs: server log file entries
    *	\brief Prints server logs.
    *	\return void
    *   
    *   \par Description
    *   Prints the contents of each log entry received from the server.
    *
    */
    void printLogs(std::vector<Server_Log_Entry> &logs);
    /*!
    *   \fn printLogs
    *	\param std::vector<Client_Log_Entry> &logs: client log file entries
    *	\brief Prints client logs.
    *	\return void 
    *   
    *   \par Description
    *   Prints the contents of each log entry read from the client machine's log file.
    *
    */
    void printLogs(std::vector<Client_Log_Entry> &logs);
    /*!
    *   \fn writeLog
    *	\param int action : Numeric code denoting the operation performed.
    *	\param int arg : Numeric argument related to the action performed.
    *	\brief Logs an operation.
    *	\return void
    *   
    *   \par Description
    *   Writes a Client_Log_Entry into the client machine's log file. 
    *   Calls shmem.logCommand to update the client's shared memory table information. 
    *
    */
    void writeLog(int action, int arg);
    /*!
    *   \fn connectedClientsInfo
    *	\param none 
    *	\brief Prints the info from the shared memory client table
    *	\return void
    *   
    *   \par Description
    *   Calls shmem.printConnectedClients to print the information of every connected client stored in the client machine's shared memory.
    *
    */
    void connectedClientsInfo();
    /*!
    *   \fn prompt
    *	\param std::string text: prompt text
    *	\brief Prints a prompt
    *	\return void
    *   
    *   \par Description
    *   Prints a standardized header prompt to stdout.
    *
    */
    void prompt(std::string text);

public:
    /*!
    *   \fn Constructor
    *	\param const int serfd : Connected server socket descriptor.
    *	\param const int lfd : Open log file descriptor.
    *	\param const sockaddr_in serAddr : Server connection info.
    *	\param const int semid : Established system semaphore set id.
    *	\brief Constructs a client.
    *	\return Client
    *   
    *   \par Description
    *   Constructs the serverSocket, logFile, and shmem objects.
    *
    */
    Client(const int serfd, const int lfd, const sockaddr_in serAddr, const int semID); 
    /*!
    *   \fn Destructor
    *	\param None.
    *	\brief Default destructor.
    *	\return void
    *   
    *   \par Description
    *   Default destructor.
    *
    */
    ~Client();
    /*!
    *   \fn run
    *	\param None.
    *	\brief Client lifetime.
    *	\return void
    *   
    *   \par Description
    *   Repeatedly reads menu selections from the user, sends operation requests to the server, and receives replies until exited by user.
    *
    */
    void run();
};

#endif