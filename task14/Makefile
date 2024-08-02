CC=gcc
CFLAGS=-Wall -Werror -Wextra
# ENTRYPOINTS=proc1.c proc2.c
SRCS=addr.c sock.c
OBJS=$(SRCS:.c=.o)
EXECUTABLES=proc1 proc2

.PHONY: all, run, clean

all: clean $(EXECUTABLES)

$(EXECUTABLES): %: %.o $(OBJS)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.a *.o $(EXECUTABLES)
