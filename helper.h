#ifndef HELPER_H
#include <stdio.h>
#include <string.h>
long factorial(int n)
{
    return (n==1 || n==0) ? 1 : n*factorial(n-1);
}
long combination(const int total,const int numtochoose)
{    

    if(numtochoose==total || numtochoose==0)
    {
	return 1;
    }
    long result=1;
    int cter=(total-numtochoose<=numtochoose) ? numtochoose+1 : total-numtochoose+1;
    for(;cter!=total+1;cter++)
    {
	result*=cter;
    }
    
    result/=factorial((total-numtochoose<=numtochoose) ? total-numtochoose : numtochoose);
    return result;
}
int compareint(const void *p1,const void *p2)
{
    if(*(int*)p1==*(int*)p2) 
	return 0;
    else
	return (*(int*)p1<*(int*)p2) ? -1 : 1;
}
void prints(char** arr,int size)
{
    int count=0;
    char temp_str[10];
    char printed_message[128];
    for(;count!=size;count++,arr++)
    {
	sprintf(temp_str,"%s,",*arr);
	if(count==0)
	{
	    strcpy(printed_message,temp_str);
	}
	else
	{
	    strcat(printed_message,temp_str);
	}
		
    }
    fprintf(stderr,"%s\n",printed_message);
}
void printd(int* arr,int size)
{
    int count=0;
    char temp_str[10];
    char printed_message[128];
    for(;count!=size;count++,arr++)
    {	
	sprintf(temp_str,"%d,",*arr);
	if(count==0)
	{
	    strcpy(printed_message,temp_str);
	}
	else
	{
	    strcat(printed_message,temp_str);
	}
    }
    fprintf(stderr,"%s\n",printed_message);
}
#endif
