Srcs := $(wildcard *.c)
Outs := $(patsubst %.c, %.o, $(Srcs))
BIN  := server

CC := gcc
CFLAGS = -Wall -g

$(BIN): $(Outs) 
	$(CC) $^ -o $@ $(CFLAGS) 

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean rebuild ALL

clean:
	$(RM) $(Outs) $(BIN)
rebuild: clean ALL

