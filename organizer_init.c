#include "organizer_init.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int init_organizer(const int option_num,char** option,int* judge_num,int* player_num)
{
    if(option_num!=2)
    {
	fprintf(stderr,"Illegal option number!\n");
	return -1;
    }
    char* endptr=NULL;
    *judge_num=strtol(option[1],&endptr,10);
    

    if(errno==ERANGE || endptr!=(option[1]+strlen(option[1])))
    {
	fprintf(stderr,"Parsing the number of judge failed!!\n");
	return -1;
    }
    
    *player_num=strtol(option[2],&endptr,10);

    if(errno==ERANGE || endptr!=(option[2]+strlen(option[2])))
    {
	fprintf(stderr,"Parsing the number of player failed!!\n");
	return -1;
    }
#ifdef DEBUG
    printf("judge num: %d, player_num: %d\n",*judge_num,*player_num);
#endif
    return 0;
}
void init_player(int* score,const int player_num)
{
    int player_counter=0;
    for(;player_counter!=player_num;player_counter++,score++)
    {
	*score=0;
    }
}
int init_judge(const int judge_num,int* pipe_tell_judge,int* pipe_listen_judge)
{
    int fork_judge_counter=0;
    for(;fork_judge_counter!=judge_num;fork_judge_counter++,pipe_tell_judge++,pipe_listen_judge++)
    {
	char judge_id[11];
       	sprintf(judge_id,"%d",fork_judge_counter+1);//judge id start from 1
	int newpipe_listen_judge[2];
	int newpipe_tell_judge[2];
	if(pipe(newpipe_listen_judge)<0 || pipe(newpipe_tell_judge)<0)
	{
	    fprintf(stderr,"pipe judge No. %d failed...\n",fork_judge_counter);
	    return -1;
	}

	int pid;
	if((pid=fork())<0)
	{
	    fprintf(stderr,"fork judge No. %s failed...\n",judge_id);
	    return -1;
	}

	else if(pid==0)//judge process
	{

	    close(newpipe_tell_judge[1]);
	    close(newpipe_listen_judge[0]);
#ifdef DEBUG
	    fprintf(stderr,"listen%d,tell%d\n",newpipe_listen_judge[0],newpipe_tell_judge[1]);
#endif
	    dup2(newpipe_tell_judge[0],0);
	    dup2(newpipe_listen_judge[1],1);
	    execl("./judge","./judge",judge_id,(char*)0);

	}
	else
	{
	    *pipe_tell_judge=newpipe_tell_judge[1];
	    *pipe_listen_judge=newpipe_listen_judge[0];
	    close(newpipe_tell_judge[0]);
	    close(newpipe_listen_judge[1]);
	}
    }
    return 0;
}
