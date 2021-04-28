
.POSIX:
.SUFFIXES:
.PHONY: all clean test

all: lisp

clean:
	rm -f lisp

lisp: lisp.cpp lisp.hpp Makefile
	c++ -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-class-memaccess -O2 -o $@ $<

test:
	./lisp unit
	./lisp load bel.lisp test.bel
	./lisp load test.std
