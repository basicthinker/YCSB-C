CC=g++
CFLAGS+=-Wall -std=c++11
INCLUDES=-I./hiredis/
HEADERS=$(wildcard *.h)
LDFLAGS=-lhiredis

all: hiredis sample

sample:	sample.cc $(HEADERS)
	$(MAKE) -C ./hiredis
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LDFLAGS) -o $@

clean:
	$(RM) sample
	$(MAKE) -C ./hiredis clean
