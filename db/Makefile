CC=g++
CFLAGS=-std=gnu++1y -c -g -Wall -fgnu-tm
INCLUDES=-I../

ifdef SITEVM_HOME
INCLUDES+=-I$(SITEVM_HOME)
endif
ifdef DUNE_HOME
INCLUDES+=-I$(DUNE_HOME)
endif
ifdef VMP_HOME
INCLUDES+=-I$(VMP_HOME)
endif

SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) $(OBJECTS)

