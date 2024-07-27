CC=gcc
CFLAGS=-Wall -Werror -Wextra
EXECUTABLES=proc1 proc2

.PHONY: all, run, clean

all: clean $(EXECUTABLES)

# run: clean $(EXECUTABLE)
# 	./$(EXECUTABLE)

$(EXECUTABLES): %: %.o common.a
	$(CC) $< -L. -lcommon -o $@

common.a: common.o
	ar -q lib$@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.a *.o $(EXECUTABLES)
