/*
 * =====================================================================================
 *
 *       Filename:  analysis.h
 *
 *    Description:  analysis.h is used for analysis html files to some link this file included.
 *
 *        Version:  1.0
 *        Created:  2014年10月26日 18时46分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  qhsong (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include<string.h>
#include<malloc.h>
#include<pthread.h>
#include "common.h"
#include "trie.h"

#define LEN_HREF 4
#define LEN_JAVASCRIPT 10 
#define LEN_HTTPFLAG 7

#define STATUS_0 0
#define STATUS_1 1
#define STATUS_2 2
#define STATUS_3 3
#define STATUS_4 4
#define STATUS_5 5
#define STATUS_6 6
#define STATUS_7 7


void* analy_run(void *arg);

void analy(void *arg);
char* trans(char *baseurl,char *url);

void get_address(char *arg,START_POINT *sp);
#endif
