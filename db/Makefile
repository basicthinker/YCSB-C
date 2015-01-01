CC=g++
CFLAGS=-std=c++11 -c -g -Wall
INCLUDES=-I../ -I/home/basicthinker/Projects/sitevm_dune-master -I/home/basicthinker/Projects/vm-persistence
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) $(OBJECTS)

