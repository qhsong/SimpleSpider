#ifndef LIBBUFTEST_H
#define LIBBUFTEST_H

#include"common.h"

void logp(int sev,const char *msg);
int write_to_server(struct bufferevent *bev , int sock , HTTP_RES *res,char *ip,int port,pthread_mutex_t *nn_mutex);
void eventcb(struct bufferevent *bev,short events,void *ptr);
void init_request(HTTP_RES *s);
void eventRead(struct bufferevent *bev,void *ptr);
int init_bvbuff(EVENT_PARM *pa,struct bufferevent *bev);
void* connserver_run(void *argv);
#endif
