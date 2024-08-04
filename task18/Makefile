CC=gcc
CFLAGS=
# CFLAGS=-Wall -Werror -Wextra
SRCS=addr.c conn.c
OBJS=$(SRCS:.c=.o)
EXECUTABLES=server client

.PHONY: all, run, clean

all: $(EXECUTABLES)

$(EXECUTABLES): %: %.o $(OBJS)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.a *.o $(EXECUTABLES)
