CC=g++
CFLAGS=-std=c++11 -Wall
INCLUDES=-I./core -I./db
LDFLAGS=
SUBDIRS=core db
OBJECTS=$(wildcard core/*.o) $(wildcard db/*.o)

all: $(SUBDIRS) ycsbc

$(SUBDIRS):
	$(MAKE) -C $@

ycsbc: ycsbc.cc $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) ycsbc.cc $(OBJECTS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done

.PHONY: $(SUBDIRS) ycsbc

