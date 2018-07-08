CC=gcc
CFLAGS=-g -Wall -pthread -std=c99
RM=rm

all: clean serv

serv: main.c queue.c utils.c logqueue.c spellchecker.h
		$(CC) $(CFLAGS) -o serv main.c queue.c utils.c logqueue.c 

clean:
		$(RM) -rf serv *.dSYM
