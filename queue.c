#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#include "spellchecker.h"

//initialize work queue
void init(Queue *q , int s){
	
	q->arr = (int*)malloc(sizeof(int)*s);
	q->front  = 0;
	q->max_size = s;
	q->rear = s - 1;
	q->size = 0;

	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->cv_empty , NULL);
	pthread_cond_init(&q->cv_full , NULL);
}

//enqueue with locks
void Enqueue(Queue *q , int value){
	
	pthread_mutex_lock(&q->mutex);

	while(q->size == q->max_size){
		pthread_cond_wait(&q->cv_full , &q->mutex);
	}
	q->rear = (q->rear + 1) % (q->max_size);
	q->arr[q->rear] = value;
	q->size++;
	
	pthread_cond_signal(&q->cv_empty);
	pthread_mutex_unlock(&q->mutex);
}

//dequeu with locks
int Dequeue(Queue *q){
	int result;
	
	pthread_mutex_lock(&q->mutex);

	while(q->size == 0){
		pthread_cond_wait(&q->cv_empty , &q->mutex);
	}
	result = q->arr[q->front];
	q->front = (q->front + 1) % (q->max_size);
	q->size--;
	
	pthread_cond_signal(&q->cv_full);
	pthread_mutex_unlock(&q->mutex);
	return result;
}