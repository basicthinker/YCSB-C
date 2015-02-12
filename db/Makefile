CC=g++-4.9
CFLAGS=-std=gnu++11 -c -g -Wall -fgnu-tm
INCLUDES=-I../ -I/home/jinglei/Projects/sitevm_dune-undo \
	-I/home/jinglei/Projects/dune
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) $(OBJECTS)

