#include "organizer_init.h"
#include "organizer_util.h"
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/select.h>
int main(int argc,char** argv)
{
    /*init input*/
    int total_judge=0;
    int	total_player=0;
    if(init_organizer(argc-1,argv,&total_judge,&total_player))
    {
	fprintf(stderr,"Initialization of judge number and player number failed.\n"
		"Usage: organizer <judge number> <player number>\n");
	return -1;
    }

    /*initialize player*/
    
    int score[total_player];
    const int total_game=combination(total_player,4);
#ifdef DEBUG
    fprintf(stderr,"organizer:total_player is %d,total game is %d\n",total_player,total_game);
#endif
    init_player(score,total_player);
    
    /*fork judge*/
    int pipe_listen_judge[total_judge];
    int pipe_tell_judge[total_judge];
    if(init_judge(total_judge,pipe_tell_judge,pipe_listen_judge)<0)
    {
	fprintf(stderr,"failed to initialize pipe to judge...\n");
	return -1;	    
    }
    
    /*producing game*/
   
    const int bufsize=512;
    char message_to_judge[bufsize];
    char message_from_judge[bufsize];
    int start_judge=total_judge;
    int finished_game=0;
    int first_player=1;
    
    for(;first_player!=total_player-2;first_player++)
    {
	
	int second_player=first_player+1;
	for(;second_player!=total_player-1;second_player++)
	{
	    
	    int third_player=second_player+1;
	    for(;third_player!=total_player;third_player++)
	    {
		
		int last_player=third_player+1;
		for(;last_player!=total_player+1;last_player++)
		{
		    
		    if(start_judge!=0)
		    {
			sprintf(message_to_judge,"%d %d %d %d\n",first_player,second_player,third_player,last_player);
#ifdef DEBUG
			fprintf(stderr,"organizer:message to judge %s\n",message_to_judge);
#endif
			write(pipe_tell_judge[start_judge-1],message_to_judge,strlen(message_to_judge));
			start_judge--;
			continue;
		    }

		    int select_judge=listen_judge(pipe_listen_judge,pipe_listen_judge+total_judge-1,total_judge);
		    read(pipe_listen_judge[select_judge-1],message_from_judge,bufsize);
		    int loser=strtol(message_from_judge,NULL,10);
#ifdef DEBUG
		    fprintf(stderr,"organizer:loser is %d\n",loser);
#endif
		    score[loser-1]--;
		    finished_game++;
#ifdef DEBUG
		    fprintf(stderr,"organizer:finished game %d,total game %d\n",finished_game,total_game);
#endif
		    sprintf(message_to_judge,"%d %d %d %d\n",first_player,second_player,third_player,last_player);
#ifdef DEBUG
		    fprintf(stderr,"organizer:message to judge %s\n",message_to_judge);
#endif
		    write(pipe_tell_judge[select_judge-1],message_to_judge,strlen(message_to_judge));
		    fflush(fdopen(pipe_tell_judge[select_judge-1],"w"));
		}
	    }
	}
    }
#ifdef DEBUG
    fprintf(stderr,"organizer:endloop\n");
#endif
    
    while(finished_game!=total_game)
    {
#ifdef DEBUG
	fprintf(stderr,"organizer:remain:\n");
#endif
	fprintf(stderr,"organizer:before select_judge,total game %d\n",total_game);
	int select_judge=listen_judge(pipe_listen_judge,pipe_listen_judge+total_judge-1,total_judge);
	read(pipe_listen_judge[select_judge-1],message_from_judge,bufsize);
	fprintf(stderr,"organizer:after select_judge,total game %d\n",total_game);
	int loser=strtol(message_from_judge,NULL,10);
#ifdef DEBUG
	fprintf(stderr,"organizer:loser is %d\n",loser);
#endif
	score[loser-1]--;
	finished_game++;
#ifdef DEBUG
	fprintf(stderr,"organizer:finished game %d total game %d\n",finished_game,total_game);
#endif
    }
    /*wait for end of judge*/
    while(total_judge!=0)
    {
	sprintf(message_to_judge,"0 0 0 0\n");
	write(pipe_tell_judge[total_judge-1],message_to_judge,strlen(message_to_judge));
	wait(0);
	total_judge--;
    }
#ifdef DEBUG
    /*print score*/
    int score_count=0;
    for(;score_count!=total_player;score_count++)
    {
	fprintf(stderr,"%d ",score[score_count]);
    }
#endif
    int order[total_player];
    int order_counter=0;
    for(;order_counter!=total_player;order_counter++)
    {
	
	order[order_counter]=order_counter+1;
	int swapper=order_counter;
	while(swapper!=0&&score[order[swapper]-1]<score[order[swapper-1]-1])
	{
	    int temp=order[swapper];
	    order[swapper]=order[swapper-1];
	    order[swapper-1]=temp;
	    swapper--;
	}
	
    }

    
    int order_count=0;
    for(;order_count!=total_player;order_count++)
    {
	printf("%d ",order[order_count]);
    }
    printf("\n");
    exit(0);
}
