#ifndef ORGANIZER_INIT_H
#define ORGANIZER_INIT_H
int init_organizer(const int option_num,char** option,int* judge_num,int* player_num);
void init_player(int* score,const int player_num);
int init_judge(const int judge_num,int pipe_tell_judge[judge_num],int pipe_listen_judge[judge_num]);
#endif
