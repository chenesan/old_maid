all: organizer	judge   player
debug:organizer.debug judge.debug player.debug

organizer:
	gcc -o organizer organizer.c organizer_init.c organizer_util.c
judge:
	gcc -o judge judge.c judge_init.c judge_util.c cards.c 
player:
	gcc -o player player.c 
organizer.debug:
	gcc -D=DEBUG -g -o organizer organizer.c organizer_init.c organizer_util.c
judge.debug:
	gcc -D=DEBUG -g -o judge judge.c judge_init.c judge_util.c cards.c 
player.debug:
	gcc -D=DEBUG -g -o player player.c 

clean:
	rm -f *.o *.debug organizer judge player
