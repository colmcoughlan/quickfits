# Set compiler to g++.
CC=gcc-mp-4.9
# Set options for the compiler
CFLAGS=-c -O3

SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=/src/quickfits.h

%.o: %.c ${DEPS}
	${CC} ${CFLAGS} -o $@ $<

all: ${OBJS}
	ar rcs libquickfits.a src/*.o
	cp src/quickfits.h .

clean:
	rm ${wildcard src/*.o} libquickfits.a quickfits.h
