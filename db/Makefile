CC=g++
CFLAGS=-std=gnu++11 -c -g -Wall -fgnu-tm
INCLUDES=-I../ -I$(SITEVM_HOME) -I$(DUNE_HOME) -I$(PLIB_HOME)
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) $(OBJECTS)

