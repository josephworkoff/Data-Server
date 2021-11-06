# Data-Server
Client/Server application for sending and receiving binary encoded data.<br>
This application implements a distributed data server that communicates with clients through sockets<br>
The server application performs operations requested by the client on a binary data file. The operations supported are shown below<br>

When the server receives a connection through its socket, it forks a child process to service that <br>

Both the server and clients keep a log file of all actions performed. These log files are shared between all server child processes and all client processes on a machine, respectively. <br>

Client processes on a machine also share access to shared memory on that machine. This shared memory implements a table containing data on all connected clients<br>
The table begins with a single integer field storing the number of clients currently connected on that machine<br>
The rest represents a table containing information on each process: its process id, total number of commands issued, connection time, and time of last command<br>
This table is updated by the client every time it issues a command to the <br>

Several pieces of data are shared between multiple processes, each representing a critical section. To guard them, both the servers and clients allocate sets of semaphores on startup that they use to synchronize access according to the readers-writers <br>

Client Commands<br>
 - D)isplay Record          : Read and display a single record from the data file. Entering '-999' displays all records. <br>
 - C)hange Record           : Update a record with new values. <br>
 - N)ew Record              : Add a new record to the file. <br>
 - S)how Server Log         : List the contents of the server's log file. <br>
 - L)Show Client Log        : List the contents of the client machine's log file. <br>
 - P)Show Connected Clients : List the contents of the client machine's process table. <br>
 - X)Exit                   : Exits the client. <br>


This application uses data representing percentage of market shares of mobile operating systems from January 2020 to January <br>
Data was taken from this source: https://www-statista-com.eu1.proxy.openathens.net/statistics/272698/global-market-share-held-by-mobile-operating-systems-since-<br>
It was cropped from January 2020 onward, and redundant fields were removed.<br>
