/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  A common struct of my program
 *
 *        Version:  1.0
 *        Created:  2014年10月31日 11时36分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  qhsong (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef COMMON_H
#define COMMON_H

#include "trie.h"

typedef struct url_rsp_s{
	int size;
	char *url[300];
}URL_RSP;

typedef struct url_req_s{
	char *url;
	char *html;
}URL_REQ;

typedef struct thread_param_s{
	char *proto;
	TRIE **head;
}THREAD_PARM;

#define END_ADDRESS "inproc://spider"
#define GLOBAL_BASE_URL "tech.qq.com"
#define LEN_GLOBAL_BASE_URL 11
#define THREAD_NUM 1
#endif

