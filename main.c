#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

#include "common.h"
#include "connserver.h"
#include "analysis.h"
#include "trie.h"
#include "bloom.h"

int main(int argc, char * argv[]){
	int itemp;
	pthread_t pt[40];
//	pthread_mutex_t isend,irecv;	
//	pthread_mutex_init(&isend,NULL);
//	pthread_mutex_init(&irecv,NULL);
	if(argc!=3){
		printf("USAGE:crawel address outputfile\n");
		return 0;
	}

	FILE *in = fopen(argv[2],"w");
	int sock = nn_socket(AF_SP,NN_PAIR);
	//int sock = 0;
	int count;
	int bytes,totalbytes;
	//TRIE *head = trie_create();
	BF *bf = bloom_create();
	assert(sock >=0);
	assert(nn_bind(sock,LISTEN_ADDRESS));
	//int sss = nn_setsockopt(sock,,NN_SNDTIMEO,(void *)1000,sizeof(int));
//	URL_REQ* url=(URL_REQ *)malloc(sizeof(URL_REQ));

	START_POINT *sp =(START_POINT *)malloc(sizeof(START_POINT));
	sp->s_add = NULL;
	//sp->s_add = (char *)malloc(sizeof(char)*1024);
	get_address(argv[1],sp);
	//printf("ip:%s|port:%d|s_add:%s",sp.ip,sp.port,sp.s_add);
	//

	int sock_conn=nn_socket(AF_SP,NN_PAIR);
	assert(sock_conn>=0);
	assert(nn_connect(sock_conn,CONNECT_ADDRESS));
//	nn_setsockopt(sock_conn,NN_PAIR,NN_SNDBUF,512,sizeof(int));
//	nn_setsockopt(sock_conn,NN_PAIR,NN_RCVBUF,512,sizeof(int));
	int countpage = 0;
	pthread_mutex_t conn_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t nn_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
	sem_t sem_empty;
	sem_init(&sem_empty,0,100000);
	sem_t sem_url_empty;
	sem_init(&sem_url_empty,1,200000);

	CONNSER_THREAD s = {sp,in,sock_conn,&countpage,&conn_mutex,&nn_mutex,&send_mutex,&sem_empty,&sem_url_empty,1};	
	int i=0;
	for(i=2;i<3;i++){
		pthread_create(&pt[0],NULL,connserver_run,(void *)&s);
	}
	void *msg = nn_allocmsg(strlen(sp->s_add)+1,0);
	memcpy(msg,sp->s_add,strlen(sp->s_add)+1);
	nn_send(sock,&msg,NN_MSG,0);

	
	THREAD_PARM parm = {bf,sock,&sem_empty,&sem_url_empty};
	pthread_create(&pt[i+1],NULL,analy_run,&parm);	
	
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
