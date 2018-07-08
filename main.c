#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "spellchecker.h"

int main(int argc , char **argv){

	int i;
	int count = 0; 

	pthread_t threadPool[WORKER_COUNT + 1]; //threads
	
	int listenfd , connfd; //listen socket and connect socket
	
	struct sockaddr_in clientaddr; //socket address struct of client
	
	t_args thread_args; //attibute of the thread

	char *port; //port 
	char *dict_name; //dictionary name
	char **word; //dictionary words

	char client_name[MAX_LEN]; //client_name
	char client_port[MAX_LEN]; //client_port

	socklen_t client_size;

	Queue queue; //work queue
	init(&queue , 20); //initialize queue
	logQueue l_queue;
	init_logQueue(&l_queue , 20);

	if(argc < 2){
		port = DEFAULT_PORT;
		dict_name = DEFAULT_DICT; //get commandline input
	}
	else if(argc < 3){
		port = argv[1];
		dict_name = DEFAULT_DICT;
	}
	else{
		port = argv[1];
		dict_name = argv[2];
	}

	listenfd = open_listenfd(port); //create listen socket

	if((word = dict_load(dict_name)) == NULL){  //load dictionary
		printf("%s\n" , "Cannot load dictionary");
	}
	else{
		printf("%s\n" , "Load dictionary successfully");
	}

	thread_args.q = &queue;
	thread_args.qu = &l_queue;  //create thread arguments
	thread_args.words = word;

	for(i = 0 ; i < WORKER_COUNT ; i++){ //create thread pools for handling service
		if((pthread_create(&threadPool[i] , NULL , &handle , &thread_args)) != 0){
			printf("%s\n" , "Cannot create threadpools");
			return EXIT_FAILURE;
		}
		else{
			printf("Create thread %ld\n" , threadPool[i]);
		}
	}
	
	if((pthread_create(&threadPool[i] , NULL , &Process_Entries , &l_queue)) != 0){ //create thread to mange logqueue
		printf("%s\n" , "Cannot create log thread");
	}

	while(1){

		client_size = sizeof(clientaddr);
		if((connfd = accept(listenfd , (struct sockaddr *) &clientaddr , &client_size))  < 0){ //accept connection from listen socket
			printf("%s\n" , "Cannot create a connection");
		}

		if(getnameinfo((struct sockaddr*) &clientaddr , sizeof(clientaddr) , 
			client_name , MAX_LEN , client_port , MAX_LEN , 0) != 0){  //pintf name and port of client
			printf("%s\n" , "Failed to get info from client");
		}
		else{
			count++;
			printf("accepted connection from %s: %s\n", client_name, client_port);
			printf("Number of threads serving now: %d\n" , count);
		}

		Enqueue(&queue , connfd); //insert socket to work queue;
		//printf("DEBUG: number of connections is %d\n", queue.size);

		if(count > WORKER_COUNT){
			printf("%s\n" , "Wait until other threads become free");
		}
	}

	for(i = 0 ; i < WORKER_COUNT ; i++){
		if(pthread_join(threadPool[i] , NULL) != 0){   //join threads
			printf("%s\n" , "Threads cannot be joined");
			return EXIT_FAILURE;
		}
	}

	free(word);
	return EXIT_SUCCESS;
}


//threads in threadpool will execute this function
//it will check fo word in dictionary to see if it's
//a match, echo the result back to client and server
//and also push the result to logqueue
void *handle(void *argument){
	
	int connectedfd;
	char buf[MAX_LEN];
	ssize_t bytes;
	char res[MAX_LEN];
	int match = 0;

	t_args *args = argument;
	Queue *queue = args->q;
	logQueue *l_queue = args->qu;
	char **words = args->words;
	while(1){

		connectedfd = Dequeue(queue);
		while((bytes = readLine(connectedfd , &buf , MAX_LEN - 1)) > 0){
			memset(res , 0 , sizeof(res));
			for(int i = 0 ; words[i] != NULL ; i++){
				if(strncmp(words[i] , buf , strlen(buf) - 2) == 0){
					strncpy(res , buf , strlen(buf) - 2);
					match = 1;
					break;
				}
			}	
			if(match == 1){
				strcat(res , " OK\n");
			}
			else{
				strncpy(res , buf , strlen(buf) - 2);
				strcat(res , " MISSPELLED\n");
			}
			write(connectedfd , res , sizeof(res));
			logging(l_queue , res);
			match = 0;
		}
		printf("connection closed\n");
		close(connectedfd);
	}
    
 }

//deque logqueue and write to log file
 void *Process_Entries(void *argument){

 	char *word;
 	logQueue *l_queue = argument;
 	FILE *fp;

 	if((fp = fopen("log.txt" , "w"))== NULL){
 		printf("%s\n" , "Cannot allocate");
 	}
 	while(1){
 		while(l_queue->size != 0){
 			word = Pop(l_queue);
 			//printf("DEBUG: number of loggs is %d\n", l_queue->size);
 			printf("%s\n" , word);
 			fputs(word , fp);
 			fflush(fp);
 		}
 	}
 	fclose(fp);
 	free(word);
 }

//push into logqueue
 void logging(logQueue *l_queue , char *word){

 	Push(l_queue , word);
 	//printf("DEBUG: number of loggs is %d\n", l_queue->size);

 }


