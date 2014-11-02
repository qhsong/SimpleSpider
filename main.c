#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<nanomsg/nn.h>
#include<nanomsg/reqrep.h>
#include<pthread.h>
#include<malloc.h>

#include "analysis.h"
#include "trie.h"
#include "common.h"

int main(){
	FILE *in = fopen("techqq2w/index.html","r");
	int i;
	int sock = nn_socket(AF_SP,NN_REQ);
	pthread_t pt[THREAD_NUM];
	char temp[1024000];
	int count;
	char *index=NULL;
	int bytes,totalbytes;
	TRIE *head = trie_create();
	count=fread(temp,1,1024000,in);
	temp[count]=0;
	assert(sock >=0);
	assert(nn_bind(sock,END_ADDRESS));
	URL_REQ* url=(URL_REQ *)malloc(sizeof(URL_REQ));

	url->url = "http://127.0.0.1/index.html";
	url->html = temp;
	for(i=0;i<THREAD_NUM;i++){
		pthread_create(&pt[i],NULL,analy_run,&head);	
	}

	while((count=nn_send(sock,&url,sizeof(URL_REQ *),NN_DONTWAIT))==EAGAIN);
		while(1){
		//sync
		bytes=nn_recv(sock,&index,sizeof(URL_RSP *),0);
		if(bytes==-1){
			printf("%s\n",nn_strerror(errno));
			fflush(stdout);
		}
		URL_RSP *q = NULL;
		q =(URL_RSP *)index;
		for(i=0;i<q->size;i++){
			printf("In main:%d %s\n",i,q->url[i]);
			//free(q->url[i]);
			fflush(stdout);
		}
		}
	for(i=0;i<THREAD_NUM;i++){
		pthread_join(pt[i],NULL);
	}
	return 0;
}
