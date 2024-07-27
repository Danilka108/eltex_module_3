CC=gcc
CFLAGS=-c -Wall
OBJECTS=main.o contacts.o person.o
EXECUTABLE=task03

all: clean build

run: clean build
	./$(EXECUTABLE)

build: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o $(EXECUTABLE)
