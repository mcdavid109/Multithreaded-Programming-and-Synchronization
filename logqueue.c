#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#include "spellchecker.h"

//initialize logqueue
void init_logQueue(logQueue *q , int s){
	
	q->str = malloc(sizeof(char *) * s);
	q->size = 0;
	q->max_size = s;
	q->front  = 0;
	q->rear = s - 1;

	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->cv_empty , NULL);
	pthread_cond_init(&q->cv_full , NULL);

}
//enqueue logqueue with lock
void Push(logQueue *q , char *words){
	
	pthread_mutex_lock(&q->mutex);

	while(q->size == q->max_size){
		pthread_cond_wait(&q->cv_full , &q->mutex);
	}
	q->rear = (q->rear + 1) % (q->max_size);
	q->str[q->rear] = words;
	q->size++;
	
	pthread_cond_signal(&q->cv_empty);
	pthread_mutex_unlock(&q->mutex);
	
}
//dequeu logqueue with lock
char *Pop(logQueue *q){

	char *res;
	
	if((res = malloc(strlen(q->str[q->front]) * sizeof(char* ) +1)) == NULL){
		printf("%s\n" , "Cannot allocate");
	}
	
	pthread_mutex_lock(&q->mutex);

	while(q->size == 0){
		pthread_cond_wait(&q->cv_empty , &q->mutex);
	}
	res = q->str[q->front];
	q->front = (q->front + 1) % (q->max_size);
	q->size--;

	pthread_cond_signal(&q->cv_full);
	pthread_mutex_unlock(&q->mutex);

	return res;
}

