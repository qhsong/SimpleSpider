#include "queuetype.h"

void initQueue(QueueType *q,int size) {
	q->base =(void **) malloc(sizeof(void *) * size);
	if(q->base == NULL){
		printf("Queue Malloc error!");
	}
	q->front = q->rear = -1;
	q->length = 0;
	q->size = size;
	pthread_mutex_init(&(q->mutex),NULL);
}

void enter(QueueType *q, void *x) {
	pthread_mutex_lock(&(q->mutex));	
	if(q->front == -1 && ((q->rear + 1)==q->size)){
		printf("Queue is full!\n");
	}else if(((q->rear + 1) % q->size) == q->front) {
		printf("Queue is full!\n");
	}else{
		q->rear = (q->rear + 1) % (q->size);
		q->base[q->rear] = x;
	}
	q->length++;
	pthread_mutex_unlock(&(q->mutex));
	fflush(stdout);
}

void *dequeue(QueueType *q) {
	pthread_mutex_lock(&(q->mutex));	
	void *tmp = NULL;	
	if(q->front == q->rear) {
		printf("Queue is empty\n");
	}else{
		tmp =  q->base[ (q->front + 1) %(q->size) ];
		q->front = (q->front + 1) % (q->size);
		q->length--;
	}
	fflush(stdout);
	pthread_mutex_unlock(&(q->mutex));
	return tmp;
}
