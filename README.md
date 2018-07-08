_______________________Network Spellchecker__________________________

Student: Tuan Anh Nguyen
Class: CIS 3207

This project provides us chance to build a network spellchecker with the aim of gaining practical experience with:
    - Multithreaded Programming and the Synchronization problem
    - TCP and Socket Programming

The spellcheker server's job is to read a sequence of words from clients. If a word is in the dictionary, it's considered
to be spelled correctly. If not, it's considered to be misspelled.

Server main task:
    - Take in commandline argument in the form ./serv port dict_name
    - If port or dict_name unspecified, use the default one
    - When server start, it load in the dictionary to an array
    - create socket descriptor to connect it with clients and push the socket to a queue
    - create a threadPool consists of multiple thread workers which will handle the jobs
    - create another seperate thread to handle writting to logfile

Worker task:
    - Deque the socket descriptor from the queue
    - Beginning read in words from user clients
    - check if match, echo the result back to both server and clients
    - push the results to the logqueue

Code structure:
#queue.c:
The work queue which will contain the socket descriptor, because multiple threads can update it at the same time. We need to 
have mutex and condition variable to synchronize the read and write

#logqueue.c:
Similar to work queue, but contains the message results

#utils:
-char *dict_load(char *name): load the dictionary into an array
-int open_listenfd(char *port): create listening socket and return it
-ssize_t readLine(int fd, void *buffer, size_t n): read in user input to a buffer

#mains:
-Do all the jobs of server and workers described above

How to execute the project:
-Press make to compile the project
-use the commandline ./serv port dictionary to create listening server
-open another terminal: enter telnet 127.0.0.1 port to test the connection

