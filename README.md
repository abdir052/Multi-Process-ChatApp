# Multi-Process-ChatApp
/*CSci4061 F2018 Project 2
*section:  1
* date: 10.10.18
* Partners: Ali Adam, Mohamed Said, Hassan Ali
* id: 5330610, 5330419, 5115657, 4570634 */

PURPOSE:

This project focuses on three OS system concepts:the use of processes to decompose an application and to provide isolation, the use of interprocess communication (IPC) to coordinate and communicate among the processes, the use of “polling” to implement asynchrony in the absence of threads. Our chat service will be provided on a single centralized server. When a user wishes to chat, they connect to the central server. The server process then creates an associated child process the user, through which users will be participating in the chat. 

HOW TO USE:

Once you are in the directory, to compile simply type in make. To run on the shell, multiple terminal windows need to be opened: one for the server, and one for as many user there are (must be less than 10 users). 

The server must be made first by using the following command: ./server
Then on a new window the user can be made by using the following command: ./client (user name)

The server supports the following commands:

1) \list: When this command is received, the SERVER creates a string with the names of all the active users and prints them. Print ’<no users>’ if there are no users currently.

2) \kick <username>:   This  command  will  be  used  to  terminate  a  particular  user’s  session.   The SERVER should terminate the session for this user by killing its child process. This user should be removed from the user list as well. 

3) \exit: The SERVER should clean up all of the users, terminate all their processes, and cleanup their pipes and wait for all child processes to terminate. Each child process should cleanup the pipes for a user and exit.

4) <any-other-text>: Any other text entered should be sent to all the active users. All users’ processes should print out this text as is with prefix "admin:".

Each user supports the following commands:

1) \list:  Same as server.  

2) \p2p <username> <message>: This command is used by a user to send messages to another particular user. 

3) \exit: When this command is used on a user’s process, the SERVER should clean-up that user, remove it from the user list, terminate the child process associated to the user, and cleanup the corresponding pipes.

4) <any-other-text>:  Any other text entered should be sent to all the active users.  All the users’ processes should print out this text as is (prefixed by the sending user).

ERROR HANDLING:

Checking for the following errors:

1) Adding more than 10 users
2) Adding user with an already existing ID 
3) Exiting and then checking the \list on server and other users
4) Killing users from server
5) Exiting server
6) Adding user with no name
7) Segmentation fault (extra credit)

ASSUMPTIONS:

1) In \p2p function it will not read messages that start with "\" it will "return message not found" 

2) Sending anything with "\" if the command isn't in the list of commands, it will broadcast it as plain text. 




