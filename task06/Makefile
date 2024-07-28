CC=gcc
CFLAGS=-Wall -Werror -Wextra
EXECUTABLES=proc1 proc2

.PHONY: all, run, clean

all: clean $(EXECUTABLES)

$(EXECUTABLES): %: %.o libmsgqueue.a
	$(CC) $< -L. -lmsgqueue -o $@

lib%.a: %.o
	ar -q $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.a *.o $(EXECUTABLES)
