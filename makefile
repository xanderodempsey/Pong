LIBS=-lzdk -lncurses -lm
DIRS=-I ZDK -L ZDK
FLAGS=-std=gnu99 -Wall -Werror -g

pong:
	gcc $(FLAGS) main.c -o pong $(DIRS) $(LIBS)
