CC=gcc
# CFLAGS=-Wall -Werror -Wextra
CFLAGS=
SRCS=main.c
OBJS=$(SRCS:.c=.o)
EXECUTABLE=task05

.PHONY: all, run, clean

all: clean $(EXECUTABLE)

run: clean $(EXECUTABLE)
	./$(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $^ -lm -o $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.bin $(EXECUTABLE)
