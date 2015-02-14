CC=g++
CFLAGS=-std=gnu++11 -c -g -Wall
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJECTS)
