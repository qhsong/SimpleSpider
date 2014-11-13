#ifndef __LISTTRIE_H__
#define __LISTTRIE_H__

#include<stdio.h>
#include<stdlib.h>

#define true 1
#define false 0

typedef struct trieTree TRIE;


struct trieTree{
	short int isEmail;
	char *str;
	TRIE *bro;
	TRIE *son;
};


TRIE* trie_create();
void trie_destroy(TRIE **head);
int trie_add(TRIE **head,char *str);
int trie_check(TRIE **head,char *str);
void trie(FILE *pool,FILE *check,FILE *result);

#endif
