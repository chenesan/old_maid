#include "cards.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void shuffle(char* cards[CARDS_NUM])
{
    srand(time(NULL));
    
    int counter;    
    for(counter=0;counter!=50;counter++)
    {
	int swap_card1=rand()%53;
	int swap_card2=rand()%53;
	char* temp=cards[swap_card2];
	cards[swap_card2]=cards[swap_card1];
	cards[swap_card1]=temp;
    }
    
}

