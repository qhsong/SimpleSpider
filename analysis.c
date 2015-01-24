/*
 * =====================================================================================
 *
 *       Filename:  analysis.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年10月26日 18时50分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  qhsong (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include<assert.h>
#include<malloc.h>
#include<stdlib.h>
#include<iconv.h>

#include "analysis.h"
#include "threadpool.h"
#include "common.h"


void* analy_run(void *arg){
	static int ic = 0;
	char *index=NULL;
	int bytes=0;
	URL_REQ *msg;
	THREAD_PARM *parm;
	parm = (THREAD_PARM *)arg;
//	TRIE **t = parm->head;
	BF *bf = parm->bf;
	int sock = parm->sock;
	pthread_mutex_t trie_mutex = PTHREAD_MUTEX_INITIALIZER;
	QueueType *qu=(QueueType *)malloc(sizeof(QueueType));
	initQueue(qu,500000);
	//iconv_t conv = iconv_open("utf-8i//IGNORE","GB2312");
	//int it,out;
	threadpool ptp = create_threadpool(1);
	void *msgrecv=NULL;
	while(1){
		bytes = nn_recv(sock,&msgrecv,NN_MSG,0);
		if(bytes>=0){
			//printf("analy_run read: %d\n",ic++);
			msg=(URL_REQ *)msgrecv;
			int h_len = evbuffer_get_length(msg->html);
			//printf("%d\n",h_len);
			//fflush(stdout);
			char *html = (char *)malloc(sizeof(char) * (h_len + 1)); 
			if(html==NULL){
				printf("Malloc error!\n");
				exit(0);
			}
			evbuffer_remove(msg->html,html,h_len);
			html[h_len]='\0';
			printf("analy recv the url %s %d\n",msg->url,h_len);


			ANALY_PARM *ap=(ANALY_PARM *)malloc(sizeof(ANALY_PARM ));
			ap->url = msg->url;
			ap->html = html;
			//ap->head = t;
			ap->bf = bf;
			ap->nn_sock = sock;
			ap->trie_mutex = &trie_mutex;
			ap->empty = parm->empty;
			ap->ss_empty = parm->ss_empty;
			ap->qu = qu;
			dispatch(ptp,analy,(void *)ap);
			//analy((void *)ap);
			//analy(msg->url,html,t,sock,parm->recv);
			evbuffer_free(msg->html);
			//msg->html = NULL;
			//free(msg);
			//nn_freemsg(msg);
			msg=NULL;
			index=NULL;
		}
			nn_freemsg(msgrecv);
	}
		//free(msg->url);
		//free(msg->html);
		//free(msg);
	return NULL;
}

void analy(void *arg){
	static int icc =0;
	ANALY_PARM *ap = (ANALY_PARM *)arg;
	char *url = ap->url;
	char* html = ap->html;
	//TRIE **head = ap->head;
	BF *bf = ap->bf;
	int nn_sock = ap->nn_sock;
	pthread_mutex_t *trie_mutex = ap->trie_mutex;

	char *outurl;
	int status = STATUS_0;
	int i = 0,j = 0;
	char temp[102400];
	int pos=0;
	pthread_mutex_lock(trie_mutex);
	//trie_add(head,url);
	bloom_add(&bf,url);
	pthread_mutex_unlock(trie_mutex);
	while(1){
		switch(status) {
		case STATUS_0:
			j = i;
			while(html[j]!='<' && html[j]!='\0') j++;
			if(html[j]=='\0'){
				//pthread_mutex_unlock(mutex);
				free(url);
				free(html);
				url = NULL;
				html = NULL;
				sem_post(ap->empty);
				free(ap);
				ap = NULL;
				return;
			}
			i = j+1;
			status = STATUS_1;
			break;
		case STATUS_1:
			if(html[i]=='a'){
				status = STATUS_2;
			}else{
				status = STATUS_0;
			}
			i++;
			break;
		case STATUS_2:
			while(1){
				if(!strncmp(html+i,"href",LEN_HREF)){	//href
					status = STATUS_3;
					i += LEN_HREF;
					break;	//break while(1)
				}else if(html[i] == '>'){
					i++;
					status = STATUS_0;
					break; //break while(1)
				}else{
					i++;
				}
			}	
			break;
		case STATUS_3:
			while(html[i]==' ') i++;
			if(html[i]=='='){
				status=STATUS_4;			
			}else{
				status = STATUS_2;
			}
			i++;
			break;
		case STATUS_4:
			while(html[i]==' ') i++;
			if(html[i]=='"'){
				status = STATUS_5;
			}else{
				status = STATUS_0;
			}
			i++;
			break;
		case STATUS_5:
			while(html[i]==' ') i++;
			if(html[i]=='#'||html[i]=='>'||html[i]=='"'){
				status = STATUS_0;
				i++;
			}else if(!strncmp(html+i,"javascript",LEN_JAVASCRIPT) || !strncmp(html+i , "https",5)){	//not https
				status = STATUS_0;
			}else{
				pos=0;
				temp[pos++]=html[i++];
				status = STATUS_6;
			}
			break;
		case STATUS_6:
			while(html[i]==' ') i++;
			while(1){
				if(html[i]=='>'){
					status = STATUS_0;
					i++;
				}else if(html[i]=='"'||html[i]=='#'||html[i]=='?'){
					status = STATUS_7;
					i++;
					break;
				}else{
					temp[pos++] = html[i++];
				}
			}
			break;
		case STATUS_7:
				temp[pos]=0;
				//printf("%s\n",temp);
				outurl = trans(url,temp);
				//trans("/a/b/index.gg","../../index.html");
				if(outurl) { 
					pthread_mutex_lock(trie_mutex);
					//if(!trie_check(head,outurl)){
					//	trie_add(head,outurl);
					if(!bloom_check(&bf,outurl)){
						bloom_add(&bf,outurl);
						//isendurl(outurl,nn_sock);
					//	printf("%s %s %s\n",url,temp,outurl);
						void *buf = nn_allocmsg(strlen(outurl)+1,0);
						memcpy(buf,outurl,strlen(outurl)+1);
						sem_wait(ap->ss_empty);
						int sendback = nn_send(nn_sock,&buf,NN_MSG,NN_DONTWAIT);
						if(sendback < 0){
							//printf("%s send failed!,error is :%s,errno is %d %d",outurl,nn_strerror(nn_errno()),nn_errno(),EAGAIN);
							enter(ap->qu,buf);
							printf("%s is enter the qu|length is %d\n",outurl,ap->qu->length);
						}else{
							int icount=0;
							while(ap->qu->length != 0 && sendback > 0 && icount!=300 ){
								buf = dequeue(ap->qu); 
								sendback = nn_send(nn_sock,&buf,NN_MSG,NN_DONTWAIT);
								icount++;
								printf("send %s,%d|length:%d\n",(char *)buf,sendback,ap->qu->length);
							}
							if(sendback < 0) {
								enter(ap->qu,buf);
								printf("re queue:%s|length:%d\n",(char*)buf,ap->qu->length);
							}
						}
						printf("trans_write %s,%s|%d\n",outurl,url,sendback);
						fflush(stdout);
						//printf("analy write:%d\n",icc++);
					}
					pthread_mutex_unlock(trie_mutex);
					free(outurl);
				}
				status = STATUS_0;
			break;
		}
	}	
}

char* trans(char *baseurl,char *url) {
	char *out = NULL,*str = url,*burl = baseurl,*index;
	if(!strncmp(str,"http://",LEN_HTTPFLAG)){
		str += LEN_HTTPFLAG;
		if(!strncmp(str,GLOBAL_BASE_URL,LEN_GLOBAL_BASE_URL)){
			index = out = (char *)malloc(1024);
			//strcat(out,"http://");
			//index += LEN_HTTPFLAG;
			*(index++)='/';
			//burl += LEN_HTTPFLAG;
			//while((*index++=*burl++)!='/');
			while(*str++!='/');
			while(*index++=*str++);
			*index = '\0';
		}else{
			//printf("Out side address:%s\n",url);
			return NULL;
		}
	}else if(!strncmp(str,"..",2)){
		char *end = burl;
		index = out = (char *)malloc(1024);
		while(*end++);
		while(*(--end)!='/');
		while(*str=='.'&&str[1]=='.'&& end != burl){
			while((*--end)!='/');
			str += 3;	//"../"length
		}
		if(end == burl && str[0]=='.' &&str[1]=='.') {
			printf("Relative address error!%s",url);
			return NULL;	//address error
		}else{
			//*(index++) = '/';
			end++;
			while(burl!=end){
				*index = *burl;
				burl++;
				index++;
			}
			//*index='/';
			//index++;
			while(*str){
				*index = *str;
				str++;
				index++;
			}
			*index='\0';
			index++;
		}
	}else if(*str=='/'){
		index = out = (char *)malloc(1024);
		//strcpy(out,"http://");
		//burl += LEN_HTTPFLAG;
		//index += LEN_HTTPFLAG;
		//while(*burl != '/'){
		//	*index = *burl;
		//	index++;
		//	burl++;
		//}
		while(*str){
			*index = *str;
			str++;
			index++;
		}
	}else{
		char *end = burl;
		index = out = (char *)malloc(1024);
		*index = '/';
		index++;
		while(*end++);
		while(*(--end)!='/');
		end++;
		//burl += LEN_HTTPFLAG;
		while(*(burl++)!='/');
		while(burl!=end){
			*index++ = *burl++;
		}
		while(*index++ = *str++);
	}
	return out;
}

void get_address(char *arg,START_POINT *sp){
	char *baseurl = (char *)malloc(strlen(arg)+10),*index;
	strcpy(baseurl,arg);
	index = baseurl;
	index += 7;
	sp->port = 0;
	sp->s_add = (char *)malloc(1024);
	int i=0;
	while(!(*index=='/' || *index==':')){
		sp->ip[i++]=*index++;
	}
	sp->ip[i]='\0';
	if(*index=='/'&& *index!='\0'){
		sp->port = 80;
	}else{
		index++;
		while(*index!='/' &&*index!='\0'){
			sp->port = sp->port * 10 + (*index - '0');
			index++;
		}
	}
	i = 0;
	if(*index=='\0'){
		sp->s_add[i++] = *index++;
	}else{
		while(*index!='\0'){
			sp->s_add[i++] = *index++;
		}
	}
	sp->s_add[i]='\0';
	index=NULL;
	free(baseurl);
	baseurl = NULL;
}
