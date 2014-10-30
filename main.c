#include<stdio.h>
#include "analysis.h"
#include "trie.h"

int main(){
	FILE *in = fopen("techqq2w/index.html","r");
	char temp[1024000];
	char **a;
	int count;
	TRIE *head = trie_create();
	count=fread(temp,1,1024000,in);
	temp[count]=0;
	analy("http://127.0.0.1/index.html",temp,a,&head);
	return 0;
}
