#include<stdio.h>
#include<string.h>
#include<event2/event.h>
#include<event2/bufferevent.h>
#include<event2/buffer.h>
#include<errno.h>
#include<event2/util.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iconv.h>
struct timeval tv;
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
void cb_write(evutil_socket_t fd,short what,void *arg){
	static int i=0;
	char temp[1000];
	if(what&EV_WRITE){
		struct timeval n;
		evutil_gettimeofday(&n,NULL);
		//printf("%d %d %d %d\n",n.tv_sec,tv.tv_sec,n.tv_usec,tv.tv_usec);
		if(i==0 ||((((n.tv_sec-tv.tv_sec)*1000+(n.tv_usec-tv.tv_usec))%200000==0)&&(n.tv_usec-tv.tv_usec)>20000) ){
		sprintf(temp,"GET / HTTP/1.1\r\nUser-Agent:Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.111 Safari/537.36\r\nHost:127.0.0.1\r\nAccept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\nConnection:keep-alive\r\n\r\n");
		send(fd,temp,strlen(temp)+1,MSG_DONTWAIT);
		i++;
		}
	}else if(what&EV_TIMEOUT){
		sprintf(temp,"GET / HTTP/1.1\r\nUser-Agent:Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.111 Safari/537.36\r\nHost:127.0.0.1\r\nAccept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\nConnection:keep-alive\r\n\r\n");
		send(fd,temp,strlen(temp)+1,MSG_DONTWAIT);
	}	
}

void cb_read(evutil_socket_t fd,short what,void *arg){
	if(what&EV_READ){
		char temp2[102400];
		int count = recv(fd,temp2,sizeof(temp2),0);
		//printf("%s",temp2);
		if(count==0){
			struct event_base *base = (struct event_base *)arg;
			event_base_loopexit(base,NULL);
		}
		printf("%d\n",count);
	}
}

int main() {
	struct sockaddr_in seradd;
	logger = fopen("log.txt","w");
	event_set_log_callback(logp);
	evutil_gettimeofday(&tv,NULL);
	bzero(&seradd ,sizeof(seradd));
	seradd.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&seradd.sin_addr);
	seradd.sin_port = htons(80);

	evutil_socket_t fd = socket(PF_INET,SOCK_STREAM,0);
	if(connect(fd,(struct sockaddr *)&seradd,sizeof(seradd))<0){
		printf("Connect Error!");
	}else{
		evutil_make_socket_nonblocking(fd);
		struct event_base *base;
		struct timeval f = {5,0};
		base = event_base_new();
		struct event *sockwrite = event_new(base,fd,EV_WRITE|EV_PERSIST|EV_TIMEOUT,cb_write,NULL);
		struct event *sockread = event_new(base,fd,EV_READ|EV_PERSIST,cb_read,(void *)base);
		event_add(sockwrite,&f);
		event_add(sockread,NULL);
		event_base_dispatch(base);
	}
}
