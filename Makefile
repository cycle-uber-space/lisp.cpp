
.POSIX:
.SUFFIXES:
.PHONY: all clean test

all: std

clean:
	rm -f std

std: std.cpp lisp.hpp Makefile
	c++ -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-class-memaccess -O2 -o $@ $<

test:
	./std unit
	./std load bel.lisp test.bel
	./std load test.std
