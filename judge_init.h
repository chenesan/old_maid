#ifndef JUDGE_INIT_H
#define JUDGE_INIT_H
#define PLAYER_NUM 4

static const char player_abcd[PLAYER_NUM][2]={"A","B","C","D"};   
int init_fifo(const char* judge_id,char** listen_fifo,char** tell_fifo);
int init_player_in_judge(const char* judge_id,int* listen_fd,int* tell_fd,char** random_key,int* player_pid,int* first_game_or_not);
int alloc_player_id(char** player_id);

#endif
