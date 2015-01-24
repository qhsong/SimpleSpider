#ifndef QUEUETYPE_H
#define QUEUETYPE_H

#include<pthread.h>
#include<malloc.h>

typedef struct queuetype_s {
	void **base;
	int front,rear;
	int size;
	int length;
	pthread_mutex_t mutex;
}QueueType;

void initQueue(QueueType *q,int size);
void enter(QueueType *q, void *x);
void *dequeue(QueueType *q);

#endif


