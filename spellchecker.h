#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include<pthread.h>

#define WORKER_COUNT 5
#define MAX_LEN 64
#define DEFAULT_PORT "9999"
#define DEFAULT_DICT "dictionary"
#define DICT_SIZE 99172
#define LISTENQ 20

//struct for work queue
typedef struct {
	int *arr;
	int rear;
	int front;
	int size;
	int max_size;

	pthread_mutex_t mutex;
	pthread_cond_t cv_full;
	pthread_cond_t cv_empty;

} Queue;

//struct for logqueue
typedef struct {
	
	char **str;
	int rear;
	int front;
	int size;
	int max_size;

	pthread_mutex_t mutex;
	pthread_cond_t cv_full;
	pthread_cond_t cv_empty;

} logQueue;

//struct fo thread
typedef struct t_args{
	
	Queue *q;
	logQueue *qu;
	char **words;

} t_args;

//workqueue
void init(Queue *q , int s);
void Enqueue(Queue *q , int value);
int Dequeue(Queue *q);

//utils
char **dict_load(char *name);
int open_listenfd(char *port);
ssize_t readLine(int fd, void *buffer, size_t n);

//handle by threadpools
void *handle(void *argument);

//logqueue
void init_logQueue(logQueue *q , int s);
void Push(logQueue *q , char *words);
char *Pop(logQueue *q);

void *Process_Entries(void *argument); //dequeue from logqueue and write to logfile
void logging(logQueue *l_queue , char *word); //push result to logqueue


#endif