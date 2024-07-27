CC=gcc
CFLAGS=-Wall -Werror -Wextra
SRCS=main.c
OBJS=$(SRCS:.c=.o)
EXECUTABLE=task01

.PHONY: all, clean

all: $(OBJS)
	$(CC) $^ -lm -o $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(EXECUTABLE)
