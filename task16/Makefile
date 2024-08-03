CC=gcc
# CFLAGS=
CFLAGS=-Wall -Werror -Wextra
SRCS=addr.c
OBJS=$(SRCS:.c=.o)
EXECUTABLES=client server

.PHONY: all, run, clean

all: clean $(EXECUTABLES)

$(EXECUTABLES): %: %.o $(OBJS)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.a *.o $(EXECUTABLES)
