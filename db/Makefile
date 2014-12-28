CC=g++
CFLAGS=-std=c++11 -c -Wall
INCLUDES=-I../ -I/home/basicthinker/Projects/sitevm_dune-master
REGSRCS=$(wildcard *.cc)
SVMSRCS=$(wildcard *.cpp)
OBJECTS=$(REGSRCS:.cc=.o) $(SVMSRCS:.cpp=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

.cpp.o:
	$(CC) $(CFLAGS) -DSVM $(INCLUDES) $< -o $@

clean:
	$(RM) $(OBJECTS)

