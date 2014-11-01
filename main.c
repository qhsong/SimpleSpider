#include<stdio.h>
#include<assert.h>
#include<nanomsg/nn.h>
#include<nanomsg/reqrep.h>
#include<pthread.h>

#include "analysis.h"
#include "trie.h"
#include "common.h"

int main(){
	FILE *in = fopen("techqq2w/index.html","r");
	int i;
	pthread_t pt[THREAD_NUM];
	char temp[1024000];
	int count;
	char *buf,*index;
	int bytes,totalbytes;
	TRIE *head = trie_create();
	count=fread(temp,1,1024000,in);
	temp[count]=0;
	int sock = nn_socket(AF_SP,NN_REQ);
	assert(sock >=0);
	assert(nn_bind(sock,END_ADDRESS));
	URL_REQ url;
	url.url = "http://127.0.0.1/index.html";
	url.html = temp;

	for(i=0;i<THREAD_NUM;i++){
		pthread_create(&pt[i],NULL,analy_run,head);	
	}

	while(nn_send(sock,&url,sizeof(URL_REQ),NN_DONTWAIT)==EAGAIN);
	for(i=0;i<THREAD_NUM;i++){
		pthread_join(pt[i],NULL);
	}
	index==NULL;
	while(1){
		bytes=nn_recv(sock,&index,1024,0);
		printf("In main:%s\n",index);
		nn_freemsg(index);
	}
	return 0;
}
