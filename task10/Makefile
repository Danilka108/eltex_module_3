CC=gcc
CFLAGS=-Wall -Werror -Wextra
# CFLAGS=
SRCS=main.c
OBJS=$(SRCS:.c=.o)
EXECUTABLE=task10

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
	./kill_processes.sh $(EXECUTABLE)
