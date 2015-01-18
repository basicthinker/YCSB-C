CC=g++-4.9
CFLAGS=-std=c++11 -g -Wall -pthread -I./
LDFLAGS=-L/home/jinglei/Projects/sitevm_dune-master/bin \
	-L/home/jinglei/Projects/dune/libdune \
	-lpthread -ltbb -lsitevm -ldune -litm
SUBDIRS=core db
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
EXEC=ycsbc

all: $(SUBDIRS) $(EXEC)

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): $(wildcard *.cc) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)

.PHONY: $(SUBDIRS) $(EXEC)

