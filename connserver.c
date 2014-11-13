#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<iconv.h>
#include<time.h>
#include<sys/time.h>
#include<malloc.h>

#include"connserver.h"

struct timeval s;
FILE *logger;

void logp(int sev,const char *msg){
	const char *s;
	switch (sev) {
	case _EVENT_LOG_DEBUG: s = "debug"; break;
	case _EVENT_LOG_MSG:   s = "msg";   break;
	case _EVENT_LOG_WARN:  s = "warn";  break;
    case _EVENT_LOG_ERR:   s = "error"; break;
    default:               s = "?";     break; /* never reached */
    }
	fprintf(logger,"[%s] %s\n", s, msg);
}

void write(void *arg) {
	struct bufferevent *out = (struct bufferevent *)arg;
	if(1){
		char temp[102400];
		sprintf(temp,"GET /test.html HTTP/1.1\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.111 Safari/537.36\r\nHost: 127.0.0.1:80\r\nAccept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\nConnection: keep-alive\r\n\r\n");
		bufferevent_write(out,temp,strlen(temp));	
	}
}

void eventcb(struct bufferevent *bev,short events,void *ptr){
	if(events&BEV_EVENT_CONNECTED) {
		char temp[300];
		printf("connected!\n");
		sprintf(temp,"GET /test.html HTTP/1.1\r\nUser-Agent:Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.111 Safari/537.36\r\nHost:127.0.0.1:80\r\nAccept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\nConnection:keep-alive\r\n\r\n");
		//printf("%s\n",temp);
		bufferevent_write(bev,temp,strlen(temp));	
	}else if(events&BEV_EVENT_ERROR){
		printf("error!,%s",evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
		bufferevent_free(bev);
	}else if(events&BEV_EVENT_TIMEOUT){
		printf("Timeout happend!\n");
		//printf("%s\n",temp);
	}else if(events&BEV_EVENT_EOF){
		printf("connection disconnect!\n");	
		bufferevent_free(bev);
		init_bvbuff((EVENT_PARM *)ptr,bev);
	}
}

void init_request(HTTP_RES *s){
	s->status = 0;
	s->clength = 0;
	s->conn = -1;
	s->nowlength = 0;
	s->ihead = 0;
	s->http_status_code = 0;
	s->html = evbuffer_new();
}


void eventRead(struct bufferevent *bev,void *ptr){
	static int c=0;
	HTTP_RES *s = ((EVENT_PARM *)ptr)->t;	
	//iconv_t conv= iconv_open("utf-8","GB2312");
	//int it,iout;
	char temp[102400];//,temp2[102400];
	int read = bufferevent_read(bev,temp,102400);
	int len;
	//it = strlen(temp);
	//iout = 102400;
	//char *in=temp,*out=temp2;
	//iconv(conv,&in,&it,&out,&iout);
	printf("%s\n",temp);
	int count=0;
	while(count<=read){
		switch(s->status){
			case 0:
				if(!strncasecmp(temp+count,"HTTP/1.1",8)){
					s->status = 1;
					count += 8;
				}else{
					count++;	
				}
				break;
			case 1:
				while(count<=read && temp[count]==' ')	count++;
				s->http_status_code = (temp[count]-'0')*100+(temp[count+1]-'0')*10 + (temp[count+2]-'0');
				count += 3;
				while(count<=read && temp[count++]!='\n');
				s->status = 2;
				break;
			case 2:
				while(count<=read){
					if(!strncasecmp(temp+count,"content-length:",15)){
						count += 15;
						while(temp[++count]==' ');
						while(temp[count]!='\r'){
							s->clength = s->clength *10+(temp[count]-'0'); 
							count++;
						}
						printf("Total:%d\n",s->clength);
						count += 2;
					}else if(!strncasecmp(temp+count,"connection:",11)){
						count += 11;
						while(temp[++count]==' ');
						if(!strncasecmp(temp+count,"close",5)){
							s->conn = 0;	
						}else{
							s->conn = 1;
						}
						while(temp[++count]!='\n');
					}else if(!strncmp(temp+count,"\r\n",2)){
						count += 2;
						s->ihead = count;
						s->status = 3;
						break;
					}else{
						while(temp[++count]!='\n');
						count++;
					}
				}
			case 3:
				len = read-count;		
				evbuffer_add(s->html,temp+count,len);
				s->nowlength += len;
				printf("s->%d\n",s->nowlength);
				if(s->nowlength == s->clength){
					//do sth.
					char *ht = (char *)malloc(s->clength+1);
					evbuffer_remove(s->html,ht,s->clength+1);
					printf("%s",ht);
					init_request(s);
					c++;
					printf("%d\n",c);
					if(c==1000){
						event_base_loopexit(((EVENT_PARM *)ptr)->base,NULL);
					}
					//printf("%d\n",s->status);
					write((void *)bev);
				}
				return;
				break;
		}
	}
//	printf("%s",temp);
	//iconv_close(conv);
}

int init_bvbuff(EVENT_PARM *pa,struct bufferevent *bev){
	bev = bufferevent_socket_new(pa->base,-1,BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);
	struct sockaddr_in sin;
	memset(&sin,0,sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(0x7f000001);
	sin.sin_port = htons(80);
	bufferevent_setwatermark(bev,EV_READ,50,0);
	bufferevent_setcb(bev,eventRead,NULL,eventcb,(void *)pa);
	bufferevent_get_enabled(bev);
	bufferevent_enable(bev,EV_READ|EV_WRITE);

	if((bufferevent_socket_connect(bev,(struct sockaddr *)&sin,sizeof(sin))) <0){
		bufferevent_free(bev);
		return -1;
	}
	struct timeval t={5,0};
	bufferevent_set_timeouts(bev,NULL,&t);
	return 0;
}

void connserver_run(void *arg){
	CONNSER_THREAD *pct =(CONNSER_THREAD *)arg;
	struct event_base *base;
	struct bufferevent *bev=NULL;
	HTTP_RES h;
	init_request(&h);
	logger = fopen("log.txt","w");
	base = event_base_new();
	event_set_log_callback(logp);
	evthread_use_pthreads();

	EVENT_PARM *pa = (EVENT_PARM *)malloc(sizeof(EVENT_PARM));
	pa->t = &h;
	pa->base = base;
	pa->st = pct->s;

	if(init_bvbuff(pa,bev) < 0){
		printf("Init error!\n");	
	}
		//pthread_t pt;
	//pthread_create(&pt,NULL,write,bev);
	event_base_loop(base,0x04);
	event_base_free(base);
	base=NULL;
	free(pa);
	return 0;
}
