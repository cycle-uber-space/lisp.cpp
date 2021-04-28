
.POSIX:
.SUFFIXES:
.PHONY: all clean

all: lisp

clean:
	rm -f lisp

lisp: lisp.cpp lisp.hpp Makefile
	c++ -std=c++11 -Wall -Wextra -Wno-unused-parameter -O2 -o $@ $<
