.PHONY:all
obj=$(wildcard *.c)
BIN=$(patsubst %.c, %, $(obj))
CC=gcc
CFLAG=-Wall -pthread -lrt
#all:$(BIN)
all:chatServer
#%.o:%.c
#	$(CC) $(CFLAG) $^ -o $@	
chatServer:chatServer.o socket.a
	$(CC) $(CFLAG) $^ -o $@	
	

clean:
	rm -rf $(BIN) *.o
