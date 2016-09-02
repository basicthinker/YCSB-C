CC=g++
CFLAGS+=-Wall -std=c++11
INCLUDES=-I../
HEADERS=$(wildcard *.h)
LDFLAGS=-lhiredis

all: hiredis sample

sample:	sample.cc ../db/redis_db.o $(HEADERS)
	$(MAKE) -C ./hiredis
	sudo $(MAKE) install -C ./hiredis
	$(MAKE) -C ../db
	$(CC) $(CFLAGS) $(INCLUDES) $< ../db/redis_db.o $(LDFLAGS) -o $@

clean:
	$(RM) sample
	$(MAKE) -C ./hiredis clean
