#include "judge_init.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int alloc_player_id(char** player_id)
{
    int init_counter=0;
    for(;init_counter!=PLAYER_NUM;init_counter++,player_id++)
    {
	*player_id=(char*)malloc(sizeof(char)*4);
    }
    return 0;
}

int init_fifo(const char* judge_id,char** listen_fifo,char** tell_fifo)
{
    
    *listen_fifo=(char*)malloc(sizeof(char)*20);
    sprintf(*listen_fifo,"./judge%s.FIFO",judge_id);
    mkfifo(*listen_fifo,0666);
    
    int init_counter=0;
    for(;init_counter!=PLAYER_NUM;init_counter++)
    {
	tell_fifo[init_counter]=(char*)malloc(sizeof(char)*20);
	sprintf(tell_fifo[init_counter],"./judge%s_%s.FIFO",judge_id,player_abcd[init_counter]);
	mkfifo(tell_fifo[init_counter],0666);
    }
    
    return 0;
}

int init_player_in_judge(const char* judge_id,int* listen_fd,int* tell_fd,char** random_key,int* player_pid,int* first_game_or_not)
{
    int fork_counter=0;
    srand(time(NULL));
    for(;fork_counter!=PLAYER_NUM;fork_counter++,tell_fd++,random_key++,player_pid++)
    {
	int pid;
	int rand_key_int=rand()%65535+1;
	*random_key=(char*)malloc(sizeof(char)*10);
	sprintf(*random_key,"%d",rand_key_int);
	char* player_index=(char*)malloc(sizeof(char)*2);
	sprintf(player_index,"%s",player_abcd[fork_counter]);
	
	if((pid=fork())<0)
	{
	    fprintf(stderr,"fork error in %d loop\n",fork_counter);
	    return -1;
	}
	else if(pid==0)
	{
	    execl("./player","./player",judge_id,player_index,*random_key,(char*)0);
	}
	else
	{
	    
	    if(*first_game_or_not==1)
	    {
		if(fork_counter==0)
		{
		    char* listen_path=(char*)malloc(sizeof(char)*20);
		    sprintf(listen_path,"judge%s.FIFO",judge_id);
		    *listen_fd=open(listen_path,O_RDONLY|O_NONBLOCK,0666);
		    free(listen_path);
    
#ifdef DEBUG
		    fprintf(stderr,"judge open %s,fd is %d\n",listen_path,*listen_fd);
#endif		
		}
	    
		char* tell_path=(char*)malloc(sizeof(char)*20);
		sprintf(tell_path,"judge%s_%s.FIFO",judge_id,player_index);
		do
		{
#ifdef DEBUG
		    fprintf(stderr,"judge open %s blocking\n",tell_path);

#endif
		    *tell_fd=open(tell_path,O_RDWR,0666);
#ifdef DEBUG
		    fprintf(stderr,"judge open %s,fd is %d\n",tell_path,*tell_fd);
		    sleep(1);
#endif
		}while(*tell_fd==-1);
		free(tell_path);
	    }
	    *player_pid=pid;

#ifdef DEBUG
	    fprintf(stderr,"player %s pid is %d\n",player_index,*player_pid);
#endif
	}
	free(player_index);
    }


    *first_game_or_not=0;
    return 0;
}
