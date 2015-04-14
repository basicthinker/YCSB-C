export SITEVM_HOME
export LIBITM_HOME

CC=g++
CFLAGS=-std=gnu++11 -g -Wall -O3 -pthread \
	-I./ -I$(SITEVM_HOME) -I$(DUNE_HOME) -I$(PLIB_HOME)
LDFLAGS=-rdynamic -L$(SITEVM_HOME)/bin -L$(PLIB_HOME) \
	-lpthread -ltbb -lsitevm -ldune -lvp -lrt
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

