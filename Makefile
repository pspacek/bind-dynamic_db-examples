COMMON_HDR = util.h instance_manager.h log.h
CC = color-gcc
CFLAGS = $(shell isc-config.sh --cflags bind9) -fpic -Wall -Wextra -std=gnu99 -ggdb3
LDFLAGS = $(shell isc-config.sh --libs bind9) -shared
OBJECTS = driver.o

%.o: %.c $(COMMON_HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJECTS)
	$(CC) $(LDFLAGS) -o sample.so $(OBJECTS)

clean:
	rm -f $(OBJECTS) sample.so
