SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c, %, $(SRCS))
CC:=gcc
FLAGS:=-g

ALL:$(OBJS)

%:%.c
	$(CC)  $^ -o $@ $(FLAGS)

clean:
	rm -rf $(OBJS)

