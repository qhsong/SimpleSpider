#include<stdio.h>
#include "analysis.h"

int main(){
	FILE *in = fopen("video.htm","r");
	char temp[1024000];
	char **a;
	int count;
	count=fread(temp,1,1024000,in);
	temp[count]=0;
	analy("test",temp,a);
	return 0;
}
