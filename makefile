# Set compiler to g++.
CC=gcc
# Set options for the compiler
CFLAGS=-c -O3

SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=/src/quickfits.h

%.o: %.c ${DEPS}
	${CC} ${CFLAGS} -o $@ $<

all: ${OBJS}
	ar rcs libquickfits.a src/*.o


clean:
	rm ${wildcard src/*.o} libquickfits.a
