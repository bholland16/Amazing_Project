CC = gcc
CFLAGS = -Wall -pedantic -std=c99
EXEC = avatar_client

# my project details
OBJS = avatar_client.o
SRCS = avatar_client.c

# Commands
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

avatar_client.o: avatar_client.c avatar_client.h amazing.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~
	rm -f *#
	rm -f ./$(EXEC)
	rm -f *.o