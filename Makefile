CC=g++-4.9
CFLAGS=-std=gnu++11 -g -Wall -pthread -I./
LDFLAGS=-rdynamic -L/home/jinglei/Projects/sitevm_dune-undo/bin \
	-lpthread -ltbb -litm -lsitevm -ldune
SUBDIRS=core db
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
EXEC=ycsbc

all: $(SUBDIRS) $(EXEC)

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): $(wildcard *.cc) $(OBJECTS)
	$(CC) $(CFLAGS) $^ /home/basicthinker/Projects/gcc/x86_64-unknown-linux-gnu/libitm/libitm.a $(LDFLAGS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)

.PHONY: $(SUBDIRS) $(EXEC)

