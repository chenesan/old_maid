#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "helper.h"
int main(int argc,char** argv)
{
	
#ifdef DEBUG
    fprintf(stderr,"call player,judgeid%s,index%s,random%s\n",argv[1],argv[2],argv[3]);
#endif

    char* judge_id=argv[1];
    char* player_index=argv[2];
    char* random_key=argv[3];

    char* tell_fifo=(char*)malloc(sizeof(char)*20);
    sprintf(tell_fifo,"judge%s.FIFO",judge_id);
    int tell_fd=open(tell_fifo,O_RDWR,777);
#ifdef DEBUG
    fprintf(stderr,"%s player to tell fifo %s,fd %d\n",player_index,tell_fifo,tell_fd);
#endif
    free(tell_fifo);

    char* listen_fifo=(char*)malloc(sizeof(char)*20);
    sprintf(listen_fifo,"judge%s_%s.FIFO",judge_id,player_index);
    int listen_fd=open(listen_fifo,O_RDONLY|O_NONBLOCK,777);
    free(listen_fifo);
    
    int cards_num=(*player_index=='A') ? 14 : 13;
#ifdef DEBUG
    fprintf(stderr,"%s cards num %d\n",player_index,cards_num);
#endif
    char* original_cards[cards_num];
    int init_cards_counter=0;
    
    FILE* listen_file=fdopen(listen_fd,"r");
    for(;init_cards_counter!=cards_num;init_cards_counter++)
    {
	original_cards[init_cards_counter]=(char*)malloc(sizeof(char)*4);
	if(fscanf(listen_file,"%s",original_cards[init_cards_counter])==EOF)
	{
	    init_cards_counter--;
#ifdef DEBUG
	    fprintf(stderr,"%s: not receive card\n",player_index);
#endif
	    sleep(1);
	    continue;
	}
	
    }
    
#ifdef DEBUG
      fprintf(stderr,"%s: before check eliminate,original cards\n:",player_index);
      prints(original_cards,cards_num);
#endif
    int count=0;
    int eliminate_num=0;
    
    for(;count!=cards_num;count++)
    {

	if(strcmp(original_cards[count],"-1")==0)
	{
	    continue;
	}	
	int eliminate_count=count+1;
	for(;eliminate_count<cards_num;eliminate_count++)
	{

	    if(strcmp(original_cards[eliminate_count],original_cards[count])==0)
	    {
		
		strcpy(original_cards[count],"-1");
		strcpy(original_cards[eliminate_count],"-1");
		eliminate_num+=2;
		break;
	    }
	}	
    }
   
    
    
    
    int original_cards_num=cards_num;
    cards_num-=eliminate_num;
    int* cards=(int*)malloc(sizeof(int)*cards_num);
    int new_cards_count=0;
    char** iter_to_original_cards=original_cards;
    int* iter_to_cards=cards;
    for(;new_cards_count!=cards_num;new_cards_count++,iter_to_original_cards++,iter_to_cards++)
    {
	
	if(strcmp(*iter_to_original_cards,"-1")==0)
	{	    
	    new_cards_count--;
	    iter_to_cards--;
	    continue;
	}
	
	*iter_to_cards=strtol(*iter_to_original_cards,NULL,10);	
    }
#ifdef DEBUG 
    fprintf(stderr,"%s: after eliminate,cards\n:",player_index);
    printd(cards,cards_num);
#endif    
    /*tell judge the number of cards*/
    char message[128];
    sprintf(message,"%s %s %d\n",player_index,random_key,cards_num);
    write(tell_fd,message,strlen(message));

    
    srand(time(NULL));
    while(1)
    {
	/*wait for draw*/
	char* type=(char*)malloc(sizeof(char)*2);
	char* temp_num=(char*)malloc(sizeof(char)*4);
	
#ifdef DEBUG
	fprintf(stderr,"%s: wait for message...\n",player_index);
#endif

	while(fscanf(listen_file,"%s %s",type,temp_num)==EOF)
	{
	    sleep(0.5);
	}
	
#ifdef DEBUG
	fprintf(stderr,"%s: get message %s %s\n",player_index,type,temp_num);
	fprintf(stderr,"%s: hold these cards:\n",player_index);
	printd(cards,cards_num);
#endif
	
	if(strcmp(type,"<")==0)
	{
	    
	    free(type);
#ifdef DEBUG
	    fprintf(stderr,"%s term to draw card\n",player_index);
#endif
	    int drawed_card_num=strtol(temp_num,NULL,10);
	    free(temp_num);
	    int drawed_id=(rand()%drawed_card_num)+1;
	    
	    sprintf(message,"%s %s %d\n",player_index,random_key,drawed_id);
	    write(tell_fd,message,strlen(message));
#ifdef DEBUG
	    fprintf(stderr,"%s:after write drawed_id to judge\n",player_index);
	    fprintf(stderr,"player_index %s,random_key %s,drawed_id %d\n",player_index,random_key,drawed_id);
#endif

	    char* card_number_str=(char*)malloc(sizeof(char)*4);	    
	    while(fscanf(listen_file,"%s",card_number_str)==EOF){}
#ifdef DEBUG	   
	    fprintf(stderr,"after judge write card_number_str to %s,card_number_str is %s\n",player_index,card_number_str);

	    fprintf(stderr,"before check eliminate,player %s have cards:\n",player_index);
	    printd(cards,cards_num);
#endif

	    int* iter_to_cards=cards;
	    int counter=0;
	    int eliminate_or_not=0;
	    int card_number_int=strtol(card_number_str,NULL,10);
	    free(card_number_str);

	    for(;counter!=cards_num;counter++,iter_to_cards++)
	    {
		if(card_number_int==*iter_to_cards)
		{
		    
		    *iter_to_cards=-1;
		    eliminate_or_not=1;
		    
		    break;
		}
	    }
#ifdef DEBUG	    
fprintf(stderr,"%s: before write eliminate_or_not %d to judge\n",player_index,eliminate_or_not);
#endif
	    sprintf(message,"%s %s %d",player_index,random_key,eliminate_or_not);
	    write(tell_fd,message,strlen(message));
	    
	    cards_num=(eliminate_or_not==1) ? (cards_num-1) : (cards_num+1);
	    if(cards_num==0)
	    {
		continue;
	    }
	    
	    
	    int* new_cards=(int*)malloc(sizeof(int)*cards_num);
	    int* iter_to_new_cards=new_cards;
	    int* iter_to_old_cards=cards;
	    for(counter=0;counter!=cards_num;counter++,iter_to_old_cards++,iter_to_new_cards++)
	    {
		if(counter==cards_num-1 && eliminate_or_not==0)
		{
		    *iter_to_new_cards=card_number_int;
		    iter_to_old_cards--;
		    continue;
		}
		if(*iter_to_old_cards==-1)
		{
		    counter--;
		    iter_to_new_cards--;
		    continue;
		}
		
		*iter_to_new_cards=*iter_to_old_cards;
	    }
	    free(cards);

	    cards=new_cards;
#ifdef DEBUG
	    fprintf(stderr,"%s: after rearrange:\n",player_index);
	    printd(cards,cards_num);
#endif
	    

	}
	else if(strcmp(type,">")==0)
	{
	    
	    free(type);
	    const int drawed_id=strtol(temp_num,NULL,10);
	    free(temp_num);
	    const int card_number=*(cards+drawed_id-1);
	   
	    
	    sprintf(message,"%s %s %d\n",player_index,random_key,card_number);
	    write(tell_fd,message,strlen(message));
#ifdef DEBUG
	    fprintf(stderr,"%s: after write card_number %d to judge\n",player_index,card_number);
#endif
	    
	    
	    if(cards_num-1==0)
	    {
		
		continue;
	    }
	    int* new_cards=(int*)malloc(sizeof(int)*(cards_num-1));
	    int* iter_to_new_cards=new_cards;
	    int* iter_to_old_cards=cards;
	    int move_index=0;
	    
	    while(move_index!=cards_num)
	    {
		if(move_index!=drawed_id-1)
		{
		    *iter_to_new_cards=*iter_to_old_cards;
		    iter_to_new_cards++;
		}


		iter_to_old_cards++;
		move_index++;
	    }
	    cards_num--;
	    free(cards);
	    cards=new_cards;
#ifdef DEBUG
	    fprintf(stderr,"%s: after rearrange,hold cards:\n",player_index);
	    printd(cards,cards_num);
#endif
    	    	    
	}
    }
    
    


    
}
