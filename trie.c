#include<limits.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include "trie.h"

#define BUFFERSIZE 320

/*This function used to find the longest common header between s1 and s2.Return the max common character index,if not, return -1; */

int find_common_head(char *s1,char *s2) {
	int index = 0;
	while(*s1!='\0' && *s2!='\0' && *s1++==*s2++) index++;
	return index;
}

TRIE* node_find(TRIE *l,char *str,int *count) {
	while(l){
		if((*count = find_common_head(l->str,str))== 0){
			l = l->bro;	
		}else{
			return l;
		}
	}
	return NULL;
}

TRIE* trie_create(){
	return NULL;
}

void trie_destroy(TRIE **head) {
	TRIE *p = *head;
	if(p->bro) trie_destroy(&(p->bro));
	if(p->son) trie_destroy(&(p->son));
	free(p->str);
	free(*head);
	*head = NULL;

}


int trie_add(TRIE **head,char *str) {
	TRIE *t = *head;
	int index=0,len1;
	char *pstr;
	while(*str){
		TRIE *l =node_find(t,str,&index);
		if(l){	/*exist the same header string.OK divided it.*/
			len1 = strlen(l->str);
			if(index < len1){	/*divied it*/
			
				/*find the end of list*/
				TRIE *tp = l->son,*temp,*q;
				
				/*new a sub node*/
				temp =(TRIE *)malloc(sizeof(TRIE));
				temp->isEmail = l->isEmail;
				q = temp;
				pstr = l->str;
				pstr += index;
				q->str = (char *)malloc(strlen(pstr)+1);
				strcpy(q->str,pstr);
				q->bro= q->son = NULL;
				
				if(tp) {
					q->son = tp;		
				}
				
				/*redirect the node*/
				l->son = temp;

				/*change l->cNode*/
				l->str[index] = '\0';
				l->isEmail = false;

				
				/*move t*/
				t = l->son;
				str += index;
			}else if(index == len1){  /*l->cNode is short*/
				str += index;
				if(!l->son){
					l->son = (TRIE *)malloc(sizeof(TRIE));
					l->son->isEmail = false;
					l->son->son = l->son->bro = NULL;
					l->son->str = (char *)malloc(strlen(str)+1);
					strcpy(l->son->str,str);
					t = l->son;
					break;
				} 
				if(*str){
					t = l->son;
				}else{
					t = l;
				}
			}
		}else{	/*new*/
			TRIE *p,*q=NULL;
			if(t){
				if(!t->bro){
					t->bro = (TRIE *)malloc(sizeof(TRIE));
					t->bro->son = t->bro->bro  = NULL;
					t->bro->str = NULL;
					q = t->bro ;
				}else{
					q = p = t;
					while(p){
						q = p;
						p = p->bro;
					}
					q->bro = (TRIE *)malloc(sizeof(TRIE));
					q = q->bro;
					q->str = NULL;
					q->bro=q->son=NULL;
				}
			}else{
				*head = (TRIE *)malloc(sizeof(TRIE));
				(*head)->str = NULL;
				(*head)->son = (*head)->bro = NULL;
				q = (*head);
			}
			if(q->str){ 
				free(q->str);
				q->str = NULL;
			}
			q->str = (char *)malloc(strlen(str)+1); 
			strcpy(q->str,str);
			t = q;
			break;
		}
	}
	t->isEmail = true;
	return 0;
}

int trie_check(TRIE **head,char *str) {
	TRIE *t = *head;
	int index = 0;
	while(*str){
		TRIE *l = node_find(t,str,&index);
		if(l && index == strlen(l->str)){
			str += index;
			if(*str){
				t = l->son;
			}else{
				t = l;
				break;
			}
		}else{
			t = NULL;
			break;
		}
	}
	if(t){
		return t->isEmail;
	}else{
		return false;
	}
}

int trimString(char *c){
	while(*c != '\r' && *c != '\n' && *c !='\0') {
		if(*c>='A' && *c<='Z') {
			*c = *c+32;
		}else if(*c < 45 || *c> 122){
			return 1;
		}
		c++;
	}
	*c = '\0';
	return 0;
}

void reverseString(char *str) {
	int len = strlen(str);
	int i;
	char temp;
	for(i=0 ; i<len/2 ; i++){
		temp = str[i];
		str[i] = str[len-i-1];
		str[len-i-1] =temp;
	}
}

void trie(FILE *pool,FILE *check,FILE *result) {
	TRIE *head = trie_create();
	char line[BUFFERSIZE];
	int exitflag=0;
	int i;
	while(fgets(line,BUFFERSIZE,pool)) {
		/*delete the useless character '\r'*/
		exitflag = trimString(line);
		if(!exitflag){
			reverseString(line);
			trie_add(&head,line);	 
		}else{
			/*printf("Error email %s",line);*/
			continue;
		}
	}
	while(fgets(line,BUFFERSIZE,check)) {

		i = 0;
		while(line[i]!='\r' && line[i]!='\n') i++;
		line[i] = '\0';
		exitflag = trimString(line);
		if(!exitflag){
			reverseString(line);
			if(trie_check(&head,line)) {
				fprintf(result,"yes\n");
			}else {
				fprintf(result,"no\n");
			}
		}
	}
	trie_destroy(&head);
}
