/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  A common struct of my program
 *
 *        Version:  1.0
 *        Created:  2014年10月31日 11时36分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  qhsong (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef COMMON_H
#define COMMON_H
#include<event2/event.h>
#include<event2/bufferevent.h>
#include<event2/buffer.h>
#include<event2/thread.h>
#include<errno.h>
#include<event2/util.h>

#include<nanomsg/nn.h>
#include<nanomsg/pair.h>
#include<pthread.h>
#include<malloc.h>
#include<semaphore.h>
#include<nanomsg/nn.h>

#include "queuetype.h"
#include "bloom.h"
#include "trie.h"

typedef struct url_rsp_s{
	int size;
	char *url[300];
}URL_RSP;

typedef struct url_req_s{
	char *url;
	struct evbuffer *html;
	//char *html;
}URL_REQ;

typedef struct thread_param_s{
	//TRIE **head;
	BF *bf;
	int sock;
	sem_t *empty;
	sem_t *ss_empty;
}THREAD_PARM;

typedef struct http_response_s{
	int status;
	int http_status_code;
	int clength;
	int nowlength;
	int conn;
	int ihead;
	char base_url[1024];
	struct evbuffer *html;
	//char *html;
}HTTP_RES;

typedef struct start_point_st{
	char ip[16];
	int port;
	char *s_add;
}START_POINT;

typedef struct connser_thread_s{
	START_POINT *s;
	FILE *wr_file;	
	int sock;
	int *count;
	pthread_mutex_t *mutex;
	pthread_mutex_t *nn_mutex;
	pthread_mutex_t *send_mutex;

	sem_t *empty;
	sem_t *ss_empty;
	int id;
}CONNSER_THREAD;

typedef struct event_parm_s{
	HTTP_RES *t;
	START_POINT *st;
	struct event_base *base; 
	int sock;
	FILE *wr_file;
	struct evbuffer *bEvbuffer;
	pthread_mutex_t *mutex;
	pthread_mutex_t *nn_mutex;
	pthread_mutex_t *send_mutex;
	sem_t *empty;
	sem_t *ss_empty;
	int *count;
	int id;
}EVENT_PARM;

typedef struct analy_parm_s{
	char *url;
	char *html;
	//TRIE **head;
	BF *bf;
	int nn_sock;
	pthread_mutex_t *trie_mutex;
	sem_t *empty;
	sem_t *ss_empty;
	QueueType *qu;
}ANALY_PARM;


#define LISTEN_ADDRESS "tcp://*:5800"
#define CONNECT_ADDRESS "tcp://127.0.0.1:5800"
#define GLOBAL_BASE_URL "tech.qq.com"
#define LEN_GLOBAL_BASE_URL 11
#define THREAD_NUM 1

#endif

