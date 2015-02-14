DUNE_HOME=/home/jinglei/Projects/dune
SITEVM_HOME=/home/jinglei/Projects/sitevm_dune
LIBITM_HOME=/home/jinglei/Projects/gcc/x86_64-unknown-linux-gnu/libitm

export SITEVM_HOME
export DUNE_HOME

CC=g++
CFLAGS=-std=gnu++11 -g -Wall -pthread -I./
LDFLAGS=-rdynamic -L$(SITEVM_HOME)/bin \
	-lpthread -ltbb -litm -lsitevm -ldune
SUBDIRS=core db
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
EXEC=ycsbc

all: $(SUBDIRS) $(EXEC)

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): $(wildcard *.cc) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LIBITM_HOME)/libitm.a $(LDFLAGS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)

.PHONY: $(SUBDIRS) $(EXEC)

