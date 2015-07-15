CC=g++
CFLAGS=-std=gnu++1y -g -O3 -Wall -pthread -I./
LDFLAGS=-rdynamic -ltbb -laerospike -lcrypto

ifdef SITEVM_HOME
CFLAGS+=-I$(SITEVM_HOME)
LDFLAGS+=-L$(SITEVM_HOME)/bin -lsitevm -lpthread -lboost_system -lz
endif
ifdef DUNE_HOME
CFLAGS+=-I$(DUNE_HOME)
LDFLAGS+=-L$(DUNE_HOME)/libdune -ldune
endif
ifdef PLIB_HOME
CFLAGS+=-I$(PLIB_HOME)
LDFLAGS+=-L$(PLIB_HOME) -lpmem -lrt
endif

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

