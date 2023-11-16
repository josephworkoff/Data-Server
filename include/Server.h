/*!	\file Server.h 
*	\brief  Server class header file.
*   A Server object represents a child data server. \n
*   It handles all communication with a single client and all operations on data requested by that client. \n
*   The operation lifetime of a Server is its run method. \n
*   Operations on the binary file are handled in the CriticalFile<Record> binFile.\n
*   Operations on the log file are handled in the CriticalFile<Server_Log_Entry>.\n
*   Communication with the client is performed in SocketConnection clientSocket.\n
*   Wherever numeric codes are used to correspond to operations, these codes are used:\n
*      Num : Operation\n
*        1 : Count\n
*        2 : Read\n
*        3 : Update\n
*        4 : Create\n
*        5 : (Log when determining operations, Client Connection when logging actions.)\n
*        6 : Client disconnection\n
*   
*/

#ifndef SERVER_H 
#define SERVER_H  

#include "SocketConnection.h"
#include "CriticalFile.h"
#include "Packets.h"



/*!
 *	\class Server
 *	\brief Child data server class
 *  \n
 *   A Server object represents a child data server. \n
 *   It handles all communication with a single client and all operations on data requested by that client. \n
 *   The operation lifetime of a Server is its run method. \n
 *   Operations on the binary file are handled in the CriticalFile<Record> binFile.\n
 *   Operations on the log file are handled in the CriticalFile<Server_Log_Entry>.\n
 *   Communication with the client is performed in SocketConnection clientSocket.\n
 */
class Server
{
private:
    /*!
    *	\var SocketConnection clientSocket - Handles communication with client.
    */
    SocketConnection clientSocket;
    /*!
    *	\var CriticalFile<Record> binFile - Performs accesses and operations on the binary file.
    */
    CriticalFile<Record> binFile;
    /*!
    *	\var CriticalFile<Server_Log_Entry> logFile - Performs accesses and operations on the log file.
    */
    CriticalFile<Server_Log_Entry> logFile;

    /*!
    *   \fn messageSwitch
    *	\param Record_Message &msg : Message struct received from client.
    *	\brief Routes commands to handlers.
    *	\return void
    *   
    *   \par Description
    *   Calls the appropriate operation handler function based on the action field of the message received.
    *   
    */
    void messageSwitch(Record_Message &msg);
    /*!
    *   \fn composeReply
    *	\param Record_Message &msg : Message struct to be sent to client.
    *	\brief 0's out struct memory.
    *	\return void
    *   
    *   \par Description
    *   Calls memset to 0 out the passed struct to prepare to be overwritten.
    *
    */
    void composeReply(Record_Message &msg);
    /*!
    *   \fn countReply
    *	\param Record_Message &msg : Message struct to be sent to client.
    *	\brief Reply to a count request.
    *	\return void
    *   
    *   \par Description
    *   Retrieves the record count of the binary file and places it in the msg.arg, or -1 on error.
    *
    */
    void countReply(Record_Message &msg);
    /*!
    *   \fn createReply
    *	\param Record_Message &msg : Message struct to be sent to client.
    *	\brief Reply to a create request.
    *	\return void
    *   
    *   \par Description
    *   Writes the record in the msg.record into the binary file.   
    *   Places the new record in the msg.record. 
    *   The month of the new record is placed in msg.arg, or -1 on error.
    *
    */
    void createReply(Record_Message &msg);
    /*!
    *   \fn readReply
    *	\param Record_Message &msg : Message struct to be sent to client.
    *	\brief Reply to a read request.
    *	\return void
    *   
    *   \par Description
    *   Reads the number record held in msg.arg into the msg.record.
    *   msg.arg = record number read, or -1 on error.
    *
    */
    void readReply(Record_Message &msg);
    /*!
    *   \fn updateReply
    *	\param Record_Message &msg : Message struct to be sent to client.
    *	\brief Reply to an update request.
    *	\return void
    *   
    *   \par Description
    *   Overwrites the record number in msg.arg with the record in msg.record.
    *   msg.arg = the record number updated, or -1 on error.
    * 
    */
    void updateReply(Record_Message &msg);
    /*!
    *   \fn logReply
    *	\param None.
    *	\brief Replies to a log request.
    *	\return void
    *   
    *   \par Description
    *   Repeatedly reads Log_Entry 's from the log file, then sends them to the client. 
    *   Log_Message.arg = 1 while there are logs being sent. 
    *   A final message is sent with arg = 0 after all logs have been sent.
    *
    */
    void logReply();
    /*!
    *   \fn writeLog
    *	\param int action : Numeric code denoting the operation performed.
    *	\param int arg : Numeric argument related to the action performed.
    *	\brief Logs an operation.
    *	\return void
    *   
    *   \par Description
    *   Writes a Server_Log_Entry into the log file. 
    *
    */
    void writeLog(int action, int arg);

public:
    /*!
    *   \fn Constructor
    *	\param const int bfd : Open binary file descriptor.
    *	\param const int clifd : Connected client socket descriptor.
    *	\param const int lfd : Open log file descriptor.
    *	\param const sockaddr_in cliAddr : Client connection info.
    *	\param const int semid : Established system semaphore set id.
    *	\brief Constructs a data server.
    *	\return Server
    *   
    *   \par Description
    *   Constructs the clientSocket, binFile, and logFile objects.
    *
    */
    Server(const int bfd, const int clifd, const int lfd, const sockaddr_in cliAddr, const int semid);
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
    ~Server();
    /*!
    *   \fn run
    *	\param None.
    *	\brief Child data server lifetime.
    *	\return void
    *   
    *   \par Description
    *   Repeatedly reads and responds to messages from the client until disconnection.
    *
    */
    void run();

};

#endif