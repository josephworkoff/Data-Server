/*!	\file SocketConnection.h 
*	\brief  SocketConnection header file.
*
*   \b Author: Joseph Workoff\n
*   \b Major: CS/SD MS\n
*   \b Creation Date: 04/01/2021\n
*   \b Due Date: 05/06/2021\n
*   \b Course: CSC552\n
*   \b Professor Name: Dr. Spiegel\n
*   \b Assignment: #3\n
*   \b Filename: SocketConnection.h\n
*   \b Purpose: Define an active socket connection wrapper class.\n
*   \n
*   A SocketConnection object represents an active connection with another process through a socket. \n
*   Messages can be sent through the encapsulated socket by calling its read and write methods.\n
*   
*/

#ifndef SOCKETCONNECTION_H 
#define SOCKETCONNECTION_H   

#include "Packets.h"

/*!
 *	\class SocketConnection
 *	\brief Socket connection wrapper class.
 *  \n
 *   A SocketConnection object represents an active connection with another process through a socket. \n
 *   Messages can be sent through the encapsulated socket by calling its read and write methods.\n
 */
class SocketConnection
{
private:
    /*!
    *	\var const int socketfd - Connected socket descriptor.
    */
    const int socketfd;
    /*!
    *	\var sockaddr_in address - Internet address information of the other process.
    */
    const sockaddr_in address;
    
public:
    /*!
    *   \fn Constructor
    *	\param const int sockfd : Connected socket descriptor.
    *	\param const sockaddr_in addr : Internet address information of the other process.
    *	\brief Constructs a SocketConnection.
    *	\return SocketConnection
    *   
    *   \par Description
    *   Constructs the SocketConnection around the active connection.
    *
    */
    SocketConnection(const int sockfd, const sockaddr_in addr);
    /*!
    *   \fn Destructor
    *	\param None.
    *	\brief Destructor. Closes socket descriptor.
    *	\return void
    *   
    *   \par Description
    *   Closes the socket descriptor on destruction.
    *
    */
    ~SocketConnection();
    /*!
    *   \fn readMessage
    *	\param Record_Message &msg: Record_Message struct to store the received message.
    *	\brief Reads a message from the socket.
    *	\return Number of bytes read, or -1 on error.
    *   
    *   \par Description
    *   Reads a message from the socket, storing the read message into the passed struct.
    *
    */
    int readMessage(Record_Message &msg);
    /*!
    *   \fn readMessage
    *	\param void* msg : pointer to message buffer
    *	\param int size : Number of bytes to read
    *	\brief Generic read.
    *	\return Number of bytes read, or -1 on error.
    *   
    *   \par Description
    *   Generalized read method. Reads a message from the socket, storing the read message into the buffer.
    *
    */
    int readMessage(void* msg, int size);
    /*!
    *   \fn writeMessage
    *	\param Record_Message &msg: Record_Message struct to be sent.
    *	\brief Writes a message to the socket.
    *	\return Number of bytes written, or -1 on error.
    *   
    *   \par Description
    *   Writes a message to the socket.
    *
    */
    int writeMessage(Record_Message &msg);
    /*!
    *   \fn writeMessage
    *	\param void* msg : pointer to message buffer
    *	\param int size : Number of bytes to write
    *	\brief Generic write.
    *	\return Number of bytes written, or -1 on error.
    *   
    *   \par Description
    *   Generalized write method. Writes a message to the socket.
    *
    */
    int writeMessage(void* msg, int size);
    /*!
    *   \fn getSocketfd
    *	\param none 
    *	\brief Socket descriptor getter.
    *	\return this->sockfd
    *   
    *   \par Description
    *   Returns socket desctiptor.
    *
    */
    int getSocketfd();
    /*!
    *   \fn getPort
    *	\param none 
    *	\brief Port getter.
    *	\return address.sin_port
    *   
    *   \par Description
    *   Returns connection port.
    *
    */
    int getPort();
    /*!
    *   \fn getipaddr
    *	\param none 
    *	\brief IP address getter.
    *	\return address.sin_addr
    *   
    *   \par Description
    *   Returns connection IP address.
    *
    */
    const char* getipaddr();

};

#endif