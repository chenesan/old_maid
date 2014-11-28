#include "judge_util.h"
#include "judge_init.h"
#include "cards.h"
#include "helper.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

char* cards[CARDS_NUM]={"0","1","1","1","1","2","2","2","2","3","3","3","3","4","4","4","4","5","5","5","5",
			"6","6","6","6","7","7","7","7","8","8","8","8","9","9","9","9","10","10","10","10",
			"11","11","11","11","12","12","12","12","13","13","13","13"};

int main(int argc,char** argv)
{
    
    
    char* judge_id=argv[1];
    
    /*initialize fifo and allocate player_id*/
    char* fifo_listen_player;
    char* fifo_tell_player[PLAYER_NUM];
    init_fifo(judge_id,&fifo_listen_player,fifo_tell_player);
    
    int listen_fd;
    int tell_fd[PLAYER_NUM];
    int first_game_or_not=1;

    while(1)
    {
	/*alloc player_id*/
	
	char* player_id[PLAYER_NUM];
	alloc_player_id(player_id);
	scanf("%s %s %s %s",player_id[0],player_id[1],player_id[2],player_id[3]);    
#ifdef DEBUG
	fprintf(stderr,"judgeinput%s %s %s %s\n",player_id[0],player_id[1],player_id[2],player_id[3]);
#endif
	if(*player_id[0]=='0')
	{
	    fprintf(stderr,"endsig");
	    break;
	}
	
	int player_pid[4];
	char* random_key[PLAYER_NUM];

	init_player_in_judge(judge_id,&listen_fd,tell_fd,random_key,player_pid,&first_game_or_not);
#ifdef DEBUG
	int debug_counter=0;
	for(;debug_counter!=PLAYER_NUM;debug_counter++)
	{
	    fprintf(stderr,"judge:player%s,pid%d,random_key%s,player_id%s,fifo_tell_path%s\n",player_abcd[debug_counter],player_pid[debug_counter],random_key[debug_counter],player_id[debug_counter],fifo_tell_player[debug_counter]);
	}
#endif
	shuffle(cards);
	int send_player=0;
	for(;send_player!=PLAYER_NUM;send_player++)
	{
	    int cards_counter=0;
	    char* send_message=(char*)malloc(sizeof(char)*256);
	    
	    for(;cards_counter!=13;cards_counter++)
	    {
		
		if(cards_counter==0)
		{
		    strcpy(send_message,cards[cards_counter+13*send_player]);
		}
		else
		{
		    strcat(send_message,cards[cards_counter+13*send_player]);
		}
		strcat(send_message," ");
		
	    }
	    
	    if(send_player==0)
	    {
		strcat(send_message,cards[CARDS_NUM-1]);
	    }	    	    	

	    write(tell_fd[send_player],send_message,strlen(send_message));
	   
#ifdef DEBUG
	    fprintf(stderr,"judge: write cards to %s\n",player_abcd[send_player]);
#endif	   	    
	    free(send_message);	    
	}

	int message_count=0;
	int player_cards_num[4];
	FILE* listen_fifo_file=fdopen(listen_fd,"r");
	
	for(;message_count!=PLAYER_NUM;message_count++)
	{
	    char* temp_player_index=(char*)malloc(sizeof(char)*256);
	    char* temp_random_key=(char*)malloc(sizeof(char)*256);
	    char* temp_cards_num=(char*)malloc(sizeof(char)*256);
	    	    
	    char message[256];
	    if(fscanf(listen_fifo_file,"%s %s %s",temp_player_index,temp_random_key,temp_cards_num)==EOF)
	    {
#ifdef DEBUG		
		fprintf(stderr,"judge:not received\n");
#endif
		sleep(1);
		message_count--;
		continue;
	    }
#ifdef DEBUG
	    fprintf(stderr,"judge:get message %s %s %s\n",temp_player_index,temp_random_key,temp_cards_num);
#endif
	    if(strcmp(random_key[*temp_player_index-'A'],temp_random_key)!=0)
	    {
		message_count--;
		continue;
	    }
	    
	    player_cards_num[*temp_player_index-'A']=strtol(temp_cards_num,NULL,10);
	    
	    free(temp_random_key);
	    free(temp_player_index);
	    free(temp_cards_num);
	    
	}
#ifdef DEBUG
	printd(player_cards_num,PLAYER_NUM);
#endif
	int player_num=PLAYER_NUM;
	int end_game[4];
	int end_game_count=0;
	for(;end_game_count!=PLAYER_NUM;end_game_count++)
	{
	    end_game[end_game_count]=0;
	}
	char message[256];
	while(player_num!=1)
	{
	    int round_counter=0;

	    for(;round_counter!=PLAYER_NUM;round_counter++)
	    {

		if(player_cards_num[round_counter]==0)
		{
		    continue;
		}
#ifdef DEBUG
		fprintf(stderr,"judge:player %s's term\n",player_abcd[round_counter]);		
#endif
		int drawed_player=(round_counter+1)%4;
		while(player_cards_num[drawed_player]==0)
		{
		    drawed_player=(drawed_player+1)%4;
		}
#ifdef DEBUG
		fprintf(stderr,"judge:drawed_player is %s\n",player_abcd[drawed_player]);
		fprintf(stderr,"judge:before write cards_num to player %s,cards num is %d\n",player_abcd[round_counter],player_cards_num[drawed_player]);
#endif
		sprintf(message,"< %d\n",player_cards_num[drawed_player]);
		write(tell_fd[round_counter],message,strlen(message));
#ifdef DEBUG
		fprintf(stderr,"judge:after write cards_num to player %s\n",player_abcd[round_counter]);
#endif		
		char* drawed_id_str=(char*)malloc(sizeof(char)*4);
		char* temp_player_index=(char*)malloc(sizeof(char)*4);
		char* temp_random_key=(char*)malloc(sizeof(char)*8);
#ifdef DEBUG
		fprintf(stderr,"judge:before read drawed_id from player %s\n",player_abcd[round_counter]);
#endif
		judge_scanf(listen_fifo_file,temp_player_index,temp_random_key,drawed_id_str,random_key[round_counter]);
#ifdef DEBUG
		fprintf(stderr,"judge:after read drawed_id from player %s\n",player_abcd[round_counter]);
		fprintf(stderr,"judge:get player_index %s,random_key %s, drawed_id %s\n",temp_player_index,temp_random_key,drawed_id_str);
		
		fprintf(stderr,"judge:before write drawed_id to player %s,drawed_id is %s\n",player_abcd[drawed_player],drawed_id_str);
#endif
		sprintf(message,"> %s\n",drawed_id_str);
		write(tell_fd[drawed_player],message,strlen(message));
#ifdef DEBUG
		fprintf(stderr,"judge:after write drawed_id to player %s,drawed_id is %s\n",player_abcd[drawed_player],drawed_id_str);
#endif
		free(drawed_id_str);
		char* card_number=(char*)malloc(sizeof(char)*4);
#ifdef DEBUG
		fprintf(stderr,"judge:before read card_number from player %s\n",player_abcd[drawed_player]);
#endif
		judge_scanf(listen_fifo_file,temp_player_index,temp_random_key,card_number,random_key[drawed_player]);
#ifdef DEBUG
		fprintf(stderr,"judge:after read card_number from player %s\n",player_abcd[drawed_player]);
#endif
		player_cards_num[drawed_player]--;
		if(player_cards_num[drawed_player]==0)
		{
		    player_num--;
		    end_game[drawed_player]=1;
		    if(player_num==1)
		    {
			break;
		    }
		}
#ifdef DEBUG
		fprintf(stderr,"judge:get player_index %s,random_key %s, card_number %s\n",temp_player_index,temp_random_key,card_number);
		fprintf(stderr,"judge:before write card_number to player %s,card_number is %s\n",player_abcd[round_counter],card_number);
#endif
		sprintf(message,"%s\n",card_number);
		write(tell_fd[round_counter],message,strlen(message));
		free(card_number);
#ifdef DEBUG
		fprintf(stderr,"judge:before read eliminate_or_not from player %s\n",player_abcd[round_counter]);
#endif
		char* eliminate_or_not=(char*)malloc(sizeof(char)*4);
		judge_scanf(listen_fifo_file,temp_player_index,temp_random_key,eliminate_or_not,random_key[round_counter]);
#ifdef DEBUG
		fprintf(stderr,"judge:after read eliminate_or_not from player %s\n",player_abcd[round_counter]);
		fprintf(stderr,"judge:get player_index %s,random_key %s, eliminate_or_not %s\n",temp_player_index,temp_random_key,eliminate_or_not);
#endif
		free(temp_random_key);
		free(temp_player_index);
		if(strcmp("1",eliminate_or_not)==0)
		{
		    player_cards_num[round_counter]--;
		    if(player_cards_num[round_counter]==0)
		    {
			player_num--;
			end_game[round_counter]=1;
			if(player_num==1)
			{
			    break;
			}
		    }
		}
		else player_cards_num[round_counter]++;
		free(eliminate_or_not);
		
#ifdef DEBUG
		int player_counter=0;
		for(;player_counter!=PLAYER_NUM;player_counter++)
		{
		    fprintf(stderr,"judge:player %s has %d cards\n",player_abcd[player_counter],player_cards_num[player_counter]);
		}
#endif		
	    }

#ifdef DEBUG
	int debug_counter=0;
	for(;debug_counter!=PLAYER_NUM;debug_counter++)
	{
	    fprintf(stderr,"judge:player%s,pid%d,random_key%s,player_id%s,fifo_tell_path%s\n",player_abcd[debug_counter],player_pid[debug_counter],random_key[debug_counter],player_id[debug_counter],fifo_tell_player[debug_counter]);
	}
#endif	    
	}

	int loser_counter=0;	
	while(end_game[loser_counter]==1)
	{
	    loser_counter++;
#ifdef DEBUG
	    fprintf(stderr,"judge: loser_counter is %d\n",loser_counter);
#endif
	}
#ifdef DEBUG
	fprintf(stderr,"judge:to organizer,loser is%s\n",player_id[loser_counter]);
#endif
	sprintf(message,"%s\n",player_id[loser_counter]);
	write(1,message,strlen(message));


	int kill_counter=0;
	char clear_buf[128];
#ifdef DEBUG
	fprintf(stderr,"judge clean listen fifo\n");
#endif
	read(listen_fd,clear_buf,128);

	for(;kill_counter!=PLAYER_NUM;kill_counter++)
	{
	    char empty_str[2];
#ifdef DEBUG
	    fprintf(stderr,"judge clean tell fifo%s\n",player_abcd[kill_counter]);
#endif
	    sprintf(empty_str,"");
	    write(tell_fd[kill_counter],empty_str,strlen(empty_str));
	    kill(player_pid[kill_counter],SIGKILL);
	    wait();
	    free(player_id[kill_counter]);
	    free(random_key[kill_counter]);
	}
	sleep(1);
    }
    
    unlink(fifo_listen_player);
#ifdef DEBUG
    fprintf(stderr,"unlink %s\n",fifo_listen_player);
#endif
    int unlink_counter=0;
    for(;unlink_counter!=PLAYER_NUM;unlink_counter++)
    {
	unlink(fifo_tell_player[unlink_counter]);
#ifdef DEBUG
	fprintf(stderr,"unlink %s\n",fifo_tell_player[unlink_counter]);
#endif
    }
    
    exit(0);
}
