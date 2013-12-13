CC = gcc -g
CFLAGS = -Wall -pedantic -std=c99
EXEC = AMStartup

# my project details
OBJS = AMStartup.o
SRCS = AMStartup.c

# Commands
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

AMStartup.o: AMStartup.c AMStartup.h amazing.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~
	rm -f *#
	rm -f ./$(EXEC)
	rm -f *.o
