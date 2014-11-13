#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

#include "common.h"
#include "connserver.h"
#include "analysis.h"
#include "trie.h"

int main(int argc, char * argv[]){
	int itemp;
	pthread_t pt[2];
	pthread_mutex_t isend,irecv;	
	pthread_mutex_init(&isend,NULL);
	pthread_mutex_init(&irecv,NULL);
	if(argc!=3){
		printf("USAGE:crawel address outputfile\n");
		return 0;
	}

	FILE *in = fopen(argv[2],"w");
	int sock = nn_socket(AF_SP,NN_PAIR);
	//int sock = 0;
	int count;
	int bytes,totalbytes;
	TRIE *head = trie_create();
	assert(sock >=0);
	assert(nn_bind(sock,END_ADDRESS));
//	URL_REQ* url=(URL_REQ *)malloc(sizeof(URL_REQ));

	char *baseurl = (char *)malloc(strlen(argv[1])+1),*index;
	strcpy(baseurl,argv[1]);
	index = baseurl;
	index += 7;
	START_POINT sp;
	sp.port = 0;
	int i=0;
	while(!(*index=='/' || *index==':')){
		sp.ip[i++]=*index++;
	}
	sp.ip[i]='\0';
	if(*index=='/'){
		sp.port = 80;
	}else{
		index++;
		while(*index!='/'){
			sp.port = sp.port * 10 + (*index - '0');
			index++;
		}
	}
	i = 0;
	while(*index!='\0'){
		sp.s_add[i++] = *index++;
	}
	sp.s_add[i]='\0';
	index=NULL;
	free(baseurl);
	baseurl = NULL;
	//printf("ip:%s|port:%d|s_add:%s",sp.ip,sp.port,sp.s_add);
	//
	CONNSER_THREAD s = {&sp,in};	
	while(nn_send(sock,sp.s_add,sizeof(char *),NN_DONTWAIT)==EAGAIN);

	pthread_create(&pt[0],NULL,connserver_run,(void *)&s);
	THREAD_PARM parm = {&head,&isend,&irecv,sock};
	for(i=0;i<THREAD_NUM;i++){
		pthread_create(&pt[1],NULL,analy_run,&parm);	
	}
	
	//while((count=nn_send(sock,&url,sizeof(URL_REQ *),NN_DONTWAIT))==EAGAIN);
	//pthread_mutex_unlock(&send);
	//	while(1){
		//sync
	//	pthread_mutex_lock(&recv);
	//	bytes=nn_recv(sock,&index,sizeof(URL_RSP *),0);
	//	if(bytes==-1){
	//		printf("%s\n",nn_strerror(errno));
	//		fflush(stdout);
	//	}
	//	URL_RSP *q = NULL;
//		q =(URL_RSP *)index;
//		for(i=0;i<q->size;i++){
//			printf("In main:%d %s\n",i,q->url[i]);
			//free(q->url[i]);
//			fflush(stdout);
//		}
//		}
	for(i=0;i<2;i++){
		pthread_join(pt[i],NULL);
	}
	return 0;
}
