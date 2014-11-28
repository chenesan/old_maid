#include "organizer_util.h"
#include <sys/select.h>
#include <stdio.h>
int listen_judge(int* fd_to_listen,int* last_fd,int total_judge)
{
    
    int select_judge=-1;
    do
    {
	fd_set read_set;
	int* fd_iter;
	
	FD_ZERO(&read_set);
	for(fd_iter=fd_to_listen;fd_iter!=last_fd+1;fd_iter++)
	{
	    FD_SET(*fd_iter,&read_set);
	}
	
	struct timeval timeout; /*used by select()*/
	timeout.tv_sec=3;
	timeout.tv_usec=0;/*set the time in select*/
	select(*last_fd+1,&read_set,0,0,&timeout);
	int judge_counter;
	
	for(fd_iter=fd_to_listen,judge_counter=1;judge_counter!=total_judge+1;fd_iter++,judge_counter++)
	{
	    if(FD_ISSET(*fd_iter,&read_set))
	    {
		select_judge=judge_counter;
#ifdef DEBUG
		fprintf(stderr,"select_judge is%d\n",judge_counter);
#endif
		break;
	    }
	}
	
    }
    while(select_judge==-1);
    return select_judge;
}
