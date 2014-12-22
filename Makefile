CC=g++
CFLAGS=-std=c++11 -Wall -pthread -g
INCLUDES=-I./core -I./db
LDFLAGS=
SUBDIRS=core db
SOURCES=$(wildcard *.cc)
OBJECTS=$(wildcard core/*.o) $(wildcard db/*.o)
EXECUTABLE=ycsbc

all: $(SUBDIRS) $(EXECUTABLE)

$(SUBDIRS):
	$(MAKE) -C $@

$(EXECUTABLE): $(SOURCES) $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) $(SOURCES) $(OBJECTS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXECUTABLE)

.PHONY: $(SUBDIRS) $(EXECUTABLE)

