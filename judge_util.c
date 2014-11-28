#include "judge_util.h"
#include <stdio.h>
#include <string.h>
void judge_scanf(FILE* listen_file,char* player_index,char* random_key,char* data,char* true_random_key)
{
    do
    {
	fscanf(listen_file,"%s %s %s",player_index,random_key,data);
    }
    while(strcmp(random_key,true_random_key)!=0);
}
