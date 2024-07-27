CC=gcc
CFLAGS=-Wall -Werror -Wextra
SRCS=main.c
OBJS=$(SRCS:.c=.o)
EXECUTABLE=task02

.PHONY: all, clean

all: $(OBJS)
	$(CC) $^ -lm -o $(EXECUTABLE)

testcmd: testcmd.o
	$(CC) testcmd.o -o testcmd

sum: sum.o
	$(CC) sum.o -lm -o sum

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(EXECUTABLE) testcmd sum
